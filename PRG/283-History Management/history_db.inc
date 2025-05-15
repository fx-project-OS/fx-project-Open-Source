<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : history_db.inc                                               //
// Version     : 24.1                                                         //
// Begin       : 2022-04-20                                                   //
// Last Change : 2024-10-09                                                   //
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
 * PF 283: History Management
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

// Already error, then leave?
if(strlen((string)$emsg))
	return;

//fxDebug('_POST');

// Literal
if($GLOBALS['fxpglobals']['lang'] == 1)	// Deutsch (German)
	$lit_history='History-';
else
	$lit_history='History ';

// Mode: Create
if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
{
	$year=0;
	$eyear=0;
	$haa=array();
	if(fxIsArray($_POST))
	{
		// Get earliest year and tables
		if(fxIsArray($_POST['nv_text1']))
		{
			reset($_POST['nv_text1']);
			$mc=key($_POST['nv_text1']);
			foreach($_POST['nv_text1'][$mc] as $ln => $val)
			{
				if(strlen((string)$val))
				{
					$va=explode('|',$val);
					$vy=(int)$va[0];
					if(!$eyear)
						$eyear=$vy;
					else
						$eyear=min($eyear,$vy);
					if(!isset($haa[$vy]))
						$haa[$vy]=array('amount'=>0, 'tables'=>array());
					$haa[$vy]['amount'] += (int)$va[2];
					$haa[$vy]['tables'][$va[1]]=(int)$va[2];
				}
			}
		}

		// Klick on... Purge Earliest Year?
		if(isset($_POST['Button_Speichern']))
			$year=$eyear;
		// Klick on... Purge Year?
		else
		{
			foreach($_POST as $pvar => $pval)
			{
				if(substr((string)$pvar,0,6) == 'purge_')
				{
					$year=(int)substr((string)$pvar,6);
					break;
				}
			}
		}
	}

	// Valid year?
	if($year && isset($haa[$year]))
	{
//fxDebug($haa[$year],'$haa[$year='.$year.']: $eyear='.$eyear, 0);
		$GLOBALS['dontAddClient']=true;

		// Possible error messages
		$tta=explode('|', meldung(3390));	// info|table definitions|table indexes|data
		$hea=explode('|', meldung(3391));	// The $var1table structures could not be determined|The temporary file$var1 could not be created|The $var1 could not be appended to the temporary file$var2|The structure for the $var1table$var2 was not found|The temporary file$var1 could not be moved to the client folder$var2|The $var1data of the table$var2 could not be deleted
		$hsa=explode('|', meldung(3392));	// The $var1tables for the year$var2 has been successfully purged|The backup has been saved into the <b>DMS</b>|The backup has been saved into the client folder$var1

		// Temporary filename + database type
		$tmp_filename=$GLOBALS['datpath'].$GLOBALS['fxpglobals']['client'].'/history-tmp-'.$GLOBALS['datetime'].'-'.$GLOBALS['fxpglobals']['person'].'.txt';
//$tmp_filename=$GLOBALS['datpath'].$GLOBALS['fxpglobals']['client'].'/history-tmp.txt';
		$dbtype=$GLOBALS['_dbtypes'][$GLOBALS['fxpglobals']['dbparam']['dbtype']]['type'];
//echo('$tmp_filename=[<b>'.$tmp_filename.'</b>], $dbtype=[<b>'.$dbtype.'</b>]<hr />');

		// Delete existing temporary file
		if(@file_exists($tmp_filename))
		{
			@chmod($tmp_filename, 0777);
			unlink($tmp_filename);
		}

		// Reading history table structures with primary keys and indexes
		dbStructure('%_history');
		if(!sizeof($GLOBALS['_dbta']))
			$emsg=strtr($hea[0].'!', array('$var1'=>$lit_history));	// The History table structures could not be determined!
//fxDebug($GLOBALS['_dbta'],'$GLOBALS[\'_dbta\']', 0);

		// File header
		if(!strlen((string)$emsg))
		{
			$hdfn='history-'.$GLOBALS['fxpglobals']['client'].'-'.$year.'.txt';
			$head=versionFileHeader($hdfn, $GLOBALS['fxpglobals']['dbparam']['version_prg'], $GLOBALS['fxpglobals']['dbparam']['versionscode_db'], $hdfn, $cp='HISTORY');
			$serr=fxSave($tmp_filename, $head, true);
			if($serr)
				$emsg=strtr($hea[1].'!', array('$var1'=>' [<b>'.$tmp_filename.'</b>]'));	// The temporary file [FILEPATH] could not be created!
		}

		// Info
		if(!strlen((string)$emsg))
		{
			$cont=$GLOBALS['nl_b'].versionFileLine('/').versionFileLine('HISTORY INFO').versionFileLine('/');
			// {YEAR}|{CLIENT}|{AMOUNT}|{TIMESTAMP}|{VERSION}|{PERSON ID}|{DATABASE NAME}|{DATABASE TYPE}
			$line=$year.'|'.$GLOBALS['fxpglobals']['client'].'|'.$haa[$year]['amount'].'|'.$GLOBALS['datetime'].'|'.$GLOBALS['fxpglobals']['dbparam']['versionscode_db'].'|'.$GLOBALS['fxpglobals']['person'].'|'.$GLOBALS['fxpglobals']['dbparam']['dbname'].'|'.$GLOBALS['fxpglobals']['dbparam']['dbtype'];
			$checksum=lineCode($line, 'HI-'.$GLOBALS['fxpglobals']['client'].'-'.$year);
			$cont .= '*H|'.$line.'|'.$checksum.$GLOBALS['nl_b'];
			// {TABLE NAME 1},{Number 1}|{TABLE NAME 2},{Number 2}|{TABLE NAME 3},{Number 3}|...|{TABLE NAME x},{Number x}
			$line='';
			foreach($haa[$year]['tables'] as $tn => $nr)
			{
				$ltn=strtolower((string)$tn);
				if(strlen((string)$line))
					$line .= '|';
				$line .= $ltn.','.$nr;
			}
			$checksum=lineCode($line, 'HT-'.$GLOBALS['fxpglobals']['client'].'-'.$year);
			$cont .= '*H|'.$line.'|'.$checksum.$GLOBALS['nl_b'];
			$serr=fxSave($tmp_filename, $cont, false, false);
			if($serr)
				$emsg=strtr($hea[2].'!', array('$var1'=>$lit_history.$tta[0], '$var2'=>' [<b>'.$tmp_filename.'</b>]'));	// The History Info could not be appended to the temporary file [FILEPATH]!
		}

		// Tables
		$idx=0;
		if(!strlen((string)$emsg))
		{
			$cont=$GLOBALS['nl_b'].versionFileLine('/').versionFileLine('HISTORY TABLES').versionFileLine('/');
			foreach($haa[$year]['tables'] as $tn => $nr)
			{
				$ltn=strtolower((string)$tn);
				if(!isset($GLOBALS['_dbta'][$ltn]))
				{
					$emsg=strtr($hea[3].'!', array('$var1'=>$lit_history, '$var2'=>' [<b>'.$ltn.'</b>]'));	// The structure for the History table [TABLENAME] was not found!
					break;
				}

				$tla=dbGetDefLine('T', $ltn, $GLOBALS['_dbta'][$ltn]);
				$cont .= '*T|'.$tla['line'].'|'.$tla['checksum'].$GLOBALS['nl_b'];

				$idx += sizeof($GLOBALS['_dbta'][$ltn]['indexes']);
			}
			if(!strlen((string)$emsg))
			{
				$serr=fxSave($tmp_filename, $cont, false, false);
				if($serr)
					$emsg=strtr($hea[2].'!', array('$var1'=>$lit_history.$tta[1], '$var2'=>' [<b>'.$tmp_filename.'</b>]'));	// The History table definitions could not be appended to the temporary file [FILEPATH]!
			}
		}

		// Indexes
		if(!strlen((string)$emsg) && $idx)
		{
			$cont=$GLOBALS['nl_b'].versionFileLine('/').versionFileLine('HISTORY INDEXES').versionFileLine('/');
			foreach($haa[$year]['tables'] as $tn => $nr)
			{
				$ltn=strtolower((string)$tn);
				if(sizeof($GLOBALS['_dbta'][$ltn]['indexes']))
				{
					$ic=0;
					foreach($GLOBALS['_dbta'][$ltn]['indexes'] as $in => $id)
					{
						$tla=dbGetDefLine('I', $ltn, $GLOBALS['_dbta'][$ltn], $ic, $id);
						$cont .= '*I|'.$tla['line'].'|'.$tla['checksum'].$GLOBALS['nl_b'];
						$ic++;
					}
				}
			}
			$serr=fxSave($tmp_filename, $cont, false, false);
			if($serr)
				$emsg=strtr($hea[2].'!', array('$var1'=>$lit_history.$tta[2], '$var2'=>' [<b>'.$tmp_filename.'</b>]'));	// The History table indexes could not be appended to the temporary file [FILEPATH]!
		}

		// Data
		if(!strlen((string)$emsg))
		{
			$cont=$GLOBALS['nl_b'].versionFileLine('/').versionFileLine('HISTORY DATA FOR CLIENT '.$GLOBALS['fxpglobals']['client'].' AND YEAR '.$year).versionFileLine('/');
			foreach($haa[$year]['tables'] as $tn => $nr)
			{
				$ltn=strtolower((string)$tn);
				// Exceptions
				$exs=fxHistoryTableExceptions($ltn);
				$sql="SELECT * FROM ".$ltn." WHERE zeitstempel LIKE '".$year."%' AND mandanten_id=".$GLOBALS['fxpglobals']['client'].$exs." ORDER BY autoid";
//echo($sql.'<hr />');
				$tca=db_values($sql,'*');
				if(fxIsArray($tca))
				{
					// ...Table header
					$tla=dbGetDefLine('D', $ltn, $GLOBALS['_dbta'][$ltn], 0, $tca);
					$cont .= '*DT|'.$tla['line'].'|'.$tla['checksum'].$GLOBALS['nl_b'];
					// ...Table data
					$dc=1;
					foreach($tca as $da)
					{
						$tla=dbGetDefLine('D', $ltn, $GLOBALS['_dbta'][$ltn], $dc, $da);
						$cont .= '*D'.$dc.'|'.$tla['line'].'|'.$tla['checksum'].$GLOBALS['nl_b'];
						$dc++;
					}
				}
			}
			$serr=fxSave($tmp_filename, $cont, false, false);
			if($serr)
				$emsg=strtr($hea[2].'!', array('$var1'=>$lit_history.$tta[3], '$var2'=>' [<b>'.$tmp_filename.'</b>]'));	// The History data could not be appended to the temporary file [FILEPATH]!
		}

		// Move temporary file with or without DMS
		if(!strlen((string)$emsg))
		{
			// DMS filename
			$dms_filename='_fxp_history_'.$year.'_'.substr((string)$GLOBALS['datetime'],0,12).'_'.$GLOBALS['fxpglobals']['dbparam']['versionscode_db'].'.txt';

			// DMS: Create new DMS element
			if($use_dms)
			{
				// ...client person id
				$sql="SELECT personen_id FROM personen WHERE personentyp=274 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$cpid=(int)db_value($sql);
				// ...upload parameters
				$uarr=array('error'=>0, 'name'=>$dms_filename, 'size'=>filesize($tmp_filename), 'new_name'=>$tmp_filename);
				// ...document id + attributes
				$seid='2_1769/'.$cpid.'.0.0';	// 2_1769=Backups (elter_art=2, dok_art=1769), $cpid=Client (elter)
				$values=array('beschreibung'=>'Backup History: '.$year);

				// Add element into DMS
				if(!isset($GLOBALS['doc']))
					$GLOBALS['doc']=new fxpDoc();
				$ra=explode('|', $GLOBALS['doc']->uploadDocument($seid, $uarr, $values));
				if($ra[0] != 'OK')	// Error
					$emsg=$ra[2];

				// Delete temporary file
				if(@file_exists($tmp_filename))
					unlink($tmp_filename);
			}
			// No DMS
			else
			{
				$ms=rename($tmp_filename, $GLOBALS['cdmspath'].$dms_filename);
				if(!$ms)
					$emsg=strtr($hea[4].'!', array('$var1'=>' [<b>'.$tmp_filename.'</b>]', '$var2'=>' [<b>'.$GLOBALS['cdmspath'].'</b>]'));	// The temporary file [FILEPATH] could not be moved to the client folder [FILEPATH]!
			}
		}

		// Delete entries in database
		if(!strlen((string)$emsg))
		{
			foreach($haa[$year]['tables'] as $tn => $nr)
			{
				$ltn=strtolower((string)$tn);
				// Exceptions
				$exs=fxHistoryTableExceptions($tn);
				$sql="DELETE FROM ".$ltn." WHERE zeitstempel LIKE '".$year."%' AND mandanten_id=".$GLOBALS['fxpglobals']['client'].$exs;
//echo($sql.'<hr />');
				$success=db_query($sql);
				if(!$success)	// Error
				{
					$emsg=strtr($hea[5].'!', array('$var1'=>$lit_history, '$var2'=>' [<b>'.$ltn.'</b>]'));	// The history data of the table [TABLENAME] could not be deleted!
					break;
				}
			}
		}

		// Success
		if(!strlen((string)$emsg))
		{
			$smsg=strtr($hsa[0].'.<br /><br />', array('$var1'=>$lit_history, '$var2'=>' <b>'.$year.'</b>'));	// The history tables for the year [YEAR] has been successfully purged.
			if($use_dms)
				$smsg .= $hsa[1].'.';																			// The backup has been saved into the DMS.
			else
				$smsg .= strtr($hsa[2].'.', array('$var1'=>' [<b>'.$GLOBALS['cdmspath'].'</b>]'));				// The backup has been saved into the client folder [FILEPATH].
			addFieldMessage('dummy',$smsg,100000);
		}
	}
	else
		error_msg(1839);	// There were no data found to work with?
}
// Mode: Display, Change or Delete
else
{
	$hmc=0;
	if(fxIsArray($_POST))
	{
		$mc=-1;
		if(fxIsArray($_POST['nv_text1']))
		{
			reset($_POST['nv_text1']);
			$mc=key($_POST['nv_text1']);
		}
		// Get all selected years?
		if($mc >= 0)
		{
			if(fxIsArray($_POST['Checkbox']) && fxIsArray($_POST['Checkbox'][$mc]))
			{
				foreach($_POST['Checkbox'][$mc] as $pvar => $pval)
				{
					if($pval)
						$fna[$_POST['nv_text1'][$mc][$pvar]]=1;
					else
						$fna[$_POST['nv_text1'][$mc][$pvar]]=0;
				}
			}
			// Restore or delete one year?
			foreach($_POST as $pvar => $pval)
			{
				if(substr((string)$pvar,0,10) == 'hm_action_')
				{
					$fna[$_POST['nv_text1'][$mc][(int)substr((string)$pvar,10)]] += 2;
					$hmc++;
					break;
				}
			}
			// Restore or delete all selected years?
			if(!$hmc && (isset($_POST['Button_Speichern']) || isset($_POST['Button_Loeschen'])))
			{
				foreach($fna as $fn => $dummy)
				{
					$fna[$fn] += 2;
					$hmc++;
				}
			}
		}
	}
//fxDebug($fna,'$fna: $hmc='.$hmc, 0);

	if($hmc)
	{
		// Possible error/success messages
		$hea=explode('|', meldung(3393)); // Error List|Error restoring the data from the $varfile$var2|You can find more information in the text file in the Log folder$var1|Error deleting the $var1file$var2 in the [<b>DMS</b>]|Error deleting the $var1file$var2 in the client folder$var3
		$hsa=explode('|', meldung(3394)); // Success List|The data of the $var1file$var2 was successfully restored into the database|The $var1file$var2 was successfully deleted from the [<b>DMS</b>]|The $var1file$var2 was successfully deleted in the client folder$var3

		// Don't write history entries
		$GLOBALS['force_nohistory']=true;

		// Commit transaction, so that we can beginn a new transaction block?
		if($GLOBALS['db_in_trans'])
			db_rollback('commit');

		$sli='';
		$eli='';
		foreach($fna as $cf => $cs)
		{
			if(($cs&2) == 2)
			{
				// Beginn transaction
				db_rollback('beginn');

				$sm='';
				$em='';

				// Restore
				if($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)
				{
					$cp=$GLOBALS['cdmspath'].$cf;
//echo('Restore: <b>'.$cp.'</b><br />');
					$err=dbExecuteFile(true, false, 'History', $cp, false);
					if($err)
						$em=strtr($hea[2].'.', array('$var1'=>' [<b>'.$GLOBALS['logpath'].'history_'.$GLOBALS['datetime'].'</b>]'));			// You can find more information in the text file in the Log folder [FILEPATH]
					else
						$sm=strtr($hsa[1].'.', array('$var1'=>$lit_history, '$var2'=>' &quot;<b>'.$cf.'</b>&quot;'));							// The data of the History file "FILEPATH" was successfully restored into the database

					if(strlen((string)$em))
						$em=strtr($hea[1].'!', array('$var1'=>$lit_client, '$var2'=>' &quot;<b>'.$cf.'</b>&quot;')).'<br /><br />&rarr; '.$em;	// Error restoring the data from the History file "FILEPATH"
				}
				// Delete
				else
				{
//echo('Delete: <b>'.$cf.'</b><br />');
					// ...Delete DMS entry
					if(class_exists('fxpDoc'))
					{
						$sql="SELECT dok_id FROM d_dok_verlauf WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND ".db_like('pfad', $cf);
						$did=(int)db_value($sql);
//fxDebug($did,$sql, 0);

						// ...DMS entry found, delete it
						if($did)
						{
							$del=false;

							if(!isset($GLOBALS['doc']))
								$GLOBALS['doc']=new fxpDoc();
							$success=$GLOBALS['doc']->deleteDoc($did, 0, -1);
							if($success)
								$sm=strtr($hsa[2].'.', array('$var1'=>$lit_history, '$var2'=>' &quot;<b>'.$cf.'</b>&quot;'));	// The History file "FILEPATH" was successfully deleted from the [DMS]
							else
								$em=strtr($hea[3].'!', array('$var1'=>$lit_history, '$var2'=>' &quot;<b>'.$cf.'</b>&quot;'));	// Error deleting the History file "FILEPATH" from the [DMS]
						}
					}

					// ...Delete file
					if(!strlen((string)$sm) && !strlen((string)$em))
					{
						$cp=$GLOBALS['cdmspath'].$cf;
//echo('Delete file: <b>'.$cp.'</b><br />');
						if(@file_exists($cp))
							unlink($cp);
						if(@file_exists($cp))
							$em=strtr($hea[4].'!', array('$var1'=>$lit_history, '$var2'=>' &quot;<b>'.$cf.'</b>&quot;', '$var3'=>' [<b>'.$GLOBALS['cdmspath'].'</b>]'));	// Error deleting the History file "FILEPATH" in the client folder [FILEPATH]
						else
							$sm=strtr($hsa[3].'.', array('$var1'=>$lit_history, '$var2'=>' &quot;<b>'.$cf.'</b>&quot;', '$var3'=>' [<b>'.$GLOBALS['cdmspath'].'</b>]'));	// The History file "FILEPATH" was successfully deleted in the client folder [FILEPATH]
					}
				}

				// Success: Commit transaction
				if(strlen((string)$sm))
				{
					$sli .= '<li>'.$sm.'<br /><br /></li>';
					db_rollback('commit');
				}
				// Error: Rollback transaction
				else
				{
					$eli .= '<li>'.$em.'<br /><br /></li>';
					db_rollback('rollback');
				}
			}
		}

		// Beginn transaction
		if(!$GLOBALS['db_in_trans'])
			db_rollback('beginn');

		// Success List
		if(strlen((string)$sli))
		{
			$smsg='<b class="green"><u>'.$hsa[0].':</u></b><br /><br /><ul style="padding-left:12px;">'.$sli.'</ul>';
			addFieldMessage('dummy',$smsg,100000);
		}
		// Error List
		if(strlen((string)$eli))
			$emsg='<b class="red"><u>'.$hea[0].':</u></b><br /><br /><ul style="padding-left:12px;">'.$eli.'</ul>';
	}
	else
	{
		// No required checkbox was marked!
		$err=true;
		error_msg(1646);
	}
//$err=true;
}

// Error message?
if(strlen((string)$emsg))
{
	$err=true;
	addFieldMessage('dummy',$emsg);
}
?>