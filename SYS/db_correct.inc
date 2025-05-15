<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : dbt_correct.inc                                              //
// Version     : 24.1                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2024-03-14                                                   //
// Author      : FeRox Management Consulting GmbH & Co. KG                    //
//               Adolf-Langer-Weg 11a, D-94036 Passau (Germany)               //
//               https://www.ferox.de - info@ferox.de                         //
// License     : GNU-GPL v3 (https://opensource.org/licenses/GPL-3.0)         //
// -------------------------------------------------------------------------- //
// fx-project - An open source PHP Project Managament Software                //
// Copyright  © FeRox Management Consulting GmbH & Co. KG                     //
// -------------------------------------------------------------------------- //
// This program is free software: you can redistribute it and/or modify       //
// it under the terms of the GNU General Public License as published by       //
// the Free Software Foundation, either version 3 of the License, or          //
// (at your option) any later version.                                        //
//                                                                            //
// This program is distributed in the hope that it will be useful,            //
// but WITHOUT ANY WARRANTY; without even the implied warranty of             //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              //
// GNU General Public License for more details.                               //
//                                                                            //
// You should have received a copy of the GNU General Public License          //
// along with this program.  If not, see <https://www.gnu.org/licenses/>.     //
//                                                                            //
// See ../LICENSE.TXT file for more information.                              //
// -------------------------------------------------------------------------- //
// LICENSING ADDENDUM:                                                        //
// Programs in the SPP (Special Programs) subfolder are coded extensions of   //
// the open source software fx-project. These programs are offered for sale   //
// by the manufacturer FeRox Management Consulting GmbH & Co. KG and require  //
// a valid key for execution. It is forbidden to resell these programs        //
// and/or keys or to pass them on free of charge or use them without the      //
// express written permission of FeRox Management Consulting GmbH & Co. KG.   //
////////////////////////////////////////////////////////////////////////////////

/**
 * @file
 * Adjust DB tables and fields to newer versions
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

if(isset($GLOBALS['from_dbt_update']) && ($GLOBALS['from_dbt_update'] === true))
{
	$correct_err=false;

	// Recreate fieldname arrays for Memo, Char, Float and Integer fields
	db_slog('*POST-ACTION: Recreate fieldname arrays for Memo, Char, Float and Integer fields');
	fxf_dbFields(true);

	if(!strlen((string)$old_version))
		$old_version = $GLOBALS['fxpglobals']['dbparam']['versionscode_db'];

	// Correct faulty process numbers
	db_slog('*POST-ACTION: Correct faulty process numbers');
	if($correct_err)
		db_slog('SKIPPED, because of a previous error!');
	else
	{
		$erc = 0;
		$cor = 0;
		$sql = "SELECT projekt_id, vorgangsnummer FROM projekte WHERE projekt_id>0 ORDER BY vorgangsnummer";
		$vna = db_values($sql, '*');
		if(fxIsArray($vna))
		{
			foreach($vna as $v)
			{
				$id = (int)$v['projekt_id'];
				$ov = trim((string)$v['vorgangsnummer']);

				$nv = '';
				$ve = explode('.', $ov);
				foreach($ve as $vl)
				{
					if(strlen((string)$nv))
						$nv .= '.';
					$nv .= substr('0000'.(int)$vl, -4);
				}

				if($ov != $nv)
				{
					$erc++;

					$sql = "UPDATE projekte SET vorgangsnummer='".$nv."' WHERE projekt_id=".$id;
					$success = db_query($sql);
					if($success)
						$cor++;
				}
			}
		}

		if(!$erc)
			db_slog('OK. No process number had to be corrected.');
		else
			db_slog($cor.' of '.$erc.' faulty process numbers have been corrected!');
	}

	// Update NULL invoice budgets
	db_slog('*POST-ACTION: Update NULL invoice budgets');
	if($correct_err)
		db_slog('SKIPPED, because of a previous error!');
	else
	{
		db_rollback('start');
		$cerr = false;

		$bs_ending_array = array('_ext', '_ext_off');
		$bs_invoicefield_array = array('fest', 'std', 'reise', 'mat');
		foreach($bs_ending_array as $bs_ending)
		{
			if($cerr)
				break;

			foreach($bs_invoicefield_array as $bs_invoicefield)
			{
				if($cerr)
					break;

				$bs_dbcolumn = 'rechnung_'.$bs_invoicefield.$bs_ending;

				$sql = "UPDATE budget_summe SET ".$bs_dbcolumn."=0 WHERE ".$bs_dbcolumn." IS NULL";
				$success = db_query($sql);
				if(!$success)
					$cerr = true;
			}
		}

		if($cerr)
		{
			db_rollback('rollback');
			$correct_err = true;
			db_slog('ERROR! Correction script failed.');
		}
		else
		{
			db_rollback('ok');
			db_slog('OK, all done.');
		}
	}

	// Adjust legacy holiday names
	db_slog('*POST-ACTION: Adjust legacy holiday names');
	if($correct_err)
		db_slog('SKIPPED, because of a previous error!');
	else
	{
		// New legacy holiday definitions
		$tda=array(
			1004=>array('day'=>'E',				'type'=>797,	'client'=>0,	'lang'=>array(1=>'Ostern - Ostersonntag',			7=>'Easter Sunday')),
			1005=>array('day'=>'E>+ 1 day',		'type'=>797,	'client'=>0,	'lang'=>array(1=>'Ostern - Ostermontag',			7=>'Easter Monday')),
			1006=>array('day'=>'0101',			'type'=>796,	'client'=>0,	'lang'=>array(1=>'Neujahr',							7=>'New Year\'s Day')),
			1007=>array('day'=>'0106',			'type'=>797,	'client'=>0,	'lang'=>array(1=>'Heilige Drei Könige',				7=>'Epiphany')),
			1008=>array('day'=>'E>+ 39 days',	'type'=>797,	'client'=>0,	'lang'=>array(1=>'Christi Himmelfahrt / Vatertag',	7=>'Christ\'s Ascension / Father\'s Day')),
			1009=>array('day'=>'E>- 2 days',	'type'=>797,	'client'=>0,	'lang'=>array(1=>'Karfreitag',						7=>'Good Friday')),
			1010=>array('day'=>'E>+ 60 days',	'type'=>797,	'client'=>0,	'lang'=>array(1=>'Fronleichnam',					7=>'Corpus Christi')),
			1012=>array('day'=>'E>- 46 days',	'type'=>797,	'client'=>0,	'lang'=>array(1=>'Aschermittwoch',					7=>'Ash Wednesday')),
			1013=>array('day'=>'E>- 48 days',	'type'=>1016,	'client'=>0,	'lang'=>array(1=>'Rosenmontag',						7=>'Collop Monday')),
			1014=>array('day'=>'E>- 47 days',	'type'=>1016,	'client'=>0,	'lang'=>array(1=>'Faschingsdienstag',				7=>'Shrove Tuesday')),
			1015=>array('day'=>'0815',			'type'=>797,	'client'=>0,	'lang'=>array(1=>'Mariä Himmelfahrt',				7=>'Marie\'s Ascension')),
			1017=>array('day'=>'0501',			'type'=>796,	'client'=>0,	'lang'=>array(1=>'Tag der Arbeit',					7=>'Labor Day')),
			1018=>array('day'=>'E>+ 49 days',	'type'=>797,	'client'=>0,	'lang'=>array(1=>'Pfingsten - Pfingstsonntag',		7=>'Pentecost - Whit Sunday')),
			1019=>array('day'=>'E>+ 50 days',	'type'=>797,	'client'=>0,	'lang'=>array(1=>'Pfingsten - Pfingstmontag',		7=>'Pentecost - Whit Monday')),
			1020=>array('day'=>'1003',			'type'=>796,	'client'=>0,	'lang'=>array(1=>'Tag der Deutschen Einheit',		7=>'German Unification Day')),
			1021=>array('day'=>'1101',			'type'=>797,	'client'=>0,	'lang'=>array(1=>'Allerheiligen',					7=>'All Saints\' Day')),
			1022=>array('day'=>'1224',			'type'=>797,	'client'=>0,	'lang'=>array(1=>'Heilig Abend',					7=>'Christmas Eve')),
			1023=>array('day'=>'1225',			'type'=>797,	'client'=>0,	'lang'=>array(1=>'1. Weihnachtsfeiertag',			7=>'Christmas Day')),
			1024=>array('day'=>'1226',			'type'=>797,	'client'=>0,	'lang'=>array(1=>'2. Weihnachtsfeiertag',			7=>'Boxing Day')),
			1025=>array('day'=>'1231',			'type'=>796,	'client'=>0,	'lang'=>array(1=>'Silvester',						7=>'New Year\'s Eve')),
			1026=>array('day'=>'1031',			'type'=>797,	'client'=>0,	'lang'=>array(1=>'Reformationstag / Halloween',		7=>'Reformation Day / Halloween')),
			1027=>array('day'=>'A>- 32 days',	'type'=>797,	'client'=>0,	'lang'=>array(1=>'Buß- und Bettag',					7=>'Prayer and Repentance')),
			1028=>array('day'=>'A>- 3 weeks',	'type'=>797,	'client'=>0,	'lang'=>array(1=>'1. Advent',						7=>'1st Advent')),
			1029=>array('day'=>'A>- 2 weeks',	'type'=>797,	'client'=>0,	'lang'=>array(1=>'2. Advent',						7=>'2nd Advent')),
			1030=>array('day'=>'A>- 1 week',	'type'=>797,	'client'=>0,	'lang'=>array(1=>'3. Advent',						7=>'3rd Advent')),
			1031=>array('day'=>'A',				'type'=>797,	'client'=>0,	'lang'=>array(1=>'4. Advent / Ewigkeitssonntag',	7=>'4th Advent / Eternity Sunday'))
		);
//fxDebug($tda,'$tda', 0);

		$tna=array();

		// Search for defined legacy holiday names
		$sql="SELECT feiertag_name, id_sprache, f_tag, name_feiertag, feiertag_art FROM feiertage_name WHERE mandanten_id=0 ORDER BY feiertag_name";
		$tmp=db_values($sql,'*');
		if(fxIsArray($tmp))
		{
//fxDebug($tmp,$sql, 0);
			foreach($tmp as $t)
			{
				$nid=(int)$t['feiertag_name'];
				$lid=(int)$t['id_sprache'];
				if(!isset($tna[$nid]))
				{
//db_slog('Client 0: '.$nid);
					if(isset($tda[$nid]))
						$tna[$nid]=$tda[$nid];
					else
						$tna[$nid]=array('day'=>$t['f_tag'], 'type'=>$t['feiertag_art'], 'client'=>0, 'lang'=>array());
				}
				if(isset($tda[$nid]['lang'][$lid]))
					$tda[$nid]['lang'][$lid]=$t['name_feiertag'];
			}
		}

		// Search for undefined legacy holiday names
		$sql  = "SELECT feiertag_name, mandanten_id FROM feiertage_mand";
		$sql .= " WHERE (feiertag_name>1003 AND feiertag_name<1032 AND feiertag_name NOT IN (1011,1016) AND feiertag_name NOT IN (SELECT feiertag_name FROM feiertage_name WHERE mandanten_id>0))";
		$sql .= " AND mandanten_id>0 ORDER BY feiertag_name";
		$tmp=db_values($sql,'*');
		if(fxIsArray($tmp))
		{
//fxDebug($tmp,$sql, 0);
			foreach($tmp as $t)
			{
				$nid=(int)$t['feiertag_name'];
				if(!isset($tna[$nid]))
				{
//db_slog('Client '.$t['mandanten_id'].': '.$nid);
					$tna[$nid]=$tda[$nid];
					$tna[$nid]['client']=(int)$t['mandanten_id'];
				}
			}
		}

		if(fxIsArray($tna))
		{
//fxDebug($tna,'$tna', 0);
			// Get all languages
			$la=fxGetAllLanguages();

			// Start Transaction
			db_rollback('start');
			$herr=false;

			foreach($tna as $nid => $na)
			{
				$now=fxNow();
				// Adjust holidays?
				$sql="SELECT feiertag_id, mandanten_id, datum FROM feiertage_mand WHERE feiertag_name=".$nid." AND mandanten_id>0 ORDER BY mandanten_id,feiertag_id";
				$tmp=db_values($sql,'*');
				if(fxIsArray($tmp))
				{
//fxDebug($tmp,$sql, 0);
					$hca=array();
					foreach($tmp as $t)
					{
						$hid=(int)$t['feiertag_id'];
						$hci=(int)$t['mandanten_id'];
						$htp=$na['type'];
						// ...Insert holiday definition for this client
						if(!isset($hca[$hci]))
						{
							$chl=array();
							// Exists a client definition for it already?
							$sql="SELECT feiertag_name, id_sprache, name_feiertag, feiertag_art FROM feiertage_name WHERE f_tag='".$na['day']."' AND mandanten_id=".$hci;
							$chd=db_values($sql,'*');
							if(fxIsArray($chd))
							{
								$hca[$hci]=(int)$chd[0]['feiertag_name'];
								$htp=$chd[0]['feiertag_art'];
								foreach($chd as $hd)
									$chl[(int)$hd['id_sprache']]=$hd['name_feiertag'];
							}
							else
							{
								$hca[$hci]=id_max('feiertage_name', 'feiertag_name');
							}
//fxDebug($chl,$sql, 0);
							foreach($la as $hli => $dummy)
							{
								$mode="insert";
								if(isset($na['lang'][$hli]))
									$hlt=$na['lang'][$hli];
								else
									$hlt='§'.$na['lang'][7];
								if(isset($chl[$hli]))
								{
									$mode="update";
									if(mb_substr((string)$chl[$hli],0,1) != '§')
										$hlt='';
								}
//echo($hli.': $mode='.$mode.', 1='.mb_substr((string)$chl[$hli],0,1).', $hlt='.$hlt.'<br />');
								if(strlen((string)$hlt))
								{
									if($mode == "insert")
										$sql=db_array2sql(array('mode'=>$mode, 'table'=>"feiertage_name", 'columns'=>array('mandanten_id'=>$hci, 'feiertag_name'=>$hca[$hci], 'id_sprache'=>$hli, 'f_tag'=>$na['day'], 'name_feiertag'=>$hlt, 'feiertag_art'=>$htp, 'transid'=>0, 'aenderungs_id'=>0, 'zeitstempel'=>$now)));
									else
										$sql=db_array2sql(array('mode'=>$mode, 'table'=>"feiertage_name", 'where'=>"mandanten_id=".$hci." AND feiertag_name=".$hca[$hci]." AND id_sprache=".$hli, 'columns'=>array('name_feiertag'=>$hlt, 'aenderungs_id'=>0, 'zeitstempel'=>$now)));
									$suc=db_query($sql);
//echo('&quot;'.$sql.'&quot;<br />&rarr; '.($suc?'<b style="color:#009f6b;">OK</b>':'<b style="color:#e1001a;">KO</b>').'<hr size=1 />'.$nl);
									if(!$suc)
									{
										$herr=true;
										break 3;
									}
								}
							}
						}
						// ...Adjust client holiday
						$sql=db_array2sql(array('mode'=>"update", 'table'=>"feiertage_mand", 'where'=>"feiertag_id=".$hid." AND mandanten_id=".$hci, 'columns'=>array('datum'=>substr((string)$t['datum'],0,8), 'feiertag_name'=>$hca[$hci])));
						$suc=db_query($sql);
//echo('&quot;'.$sql.'&quot;<br />&rarr; '.($suc?'<b style="color:#009f6b;">OK</b>':'<b style="color:#e1001a;">KO</b>').'<hr size=1 />'.$nl);
						if(!$suc)
						{
							$herr=true;
							break 2;
						}
					}
				}
				// Delete holiday name?
				if(!$na['client'])
				{
					$sql=db_array2sql(array('mode'=>"delete", 'table'=>"feiertage_name", 'where'=>"feiertag_name=".$nid." AND mandanten_id=0"));
					$suc=db_query($sql);
//echo('&quot;'.$sql.'&quot;<br />&rarr; '.($suc?'<b style="color:#009f6b;">OK</b>':'<b style="color:#e1001a;">KO</b>').'<hr size=1 />'.$nl);
					if(!$suc)
					{
						$herr=true;
						break;
					}
				}
			}

			// Transaction Rollback?
			if($herr)
			{
				db_rollback('rollback');
				$correct_err=true;
				db_slog('ERROR! Correction script failed.');
			}
			else
			{
				db_rollback('commit');
				db_slog('OK, all done.');
			}
		}
		else
			db_slog('OK. No legacy holiday names had to be adjusted.');
	}
}
?>