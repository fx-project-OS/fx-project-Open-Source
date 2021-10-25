<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : feiertage_sind_db.inc                                        //
// Version     : 21.2                                                         //
// Begin       : 2020-09-24                                                   //
// Last Change : 2020-09-29                                                   //
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
 * PF 115: Bank Holidays - Database file for saving the bank holidays
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

if(!$err && fxIsArray($hva) && fxIsArray($_POST) && isset($_POST['Button_Multi']))
{
	// Success counter variables
	$sc=0;
	$sl='';

	// Loop through holiday...
	$na=array();	// Used names
	foreach($hva as $hl => $ha)
	{
		if(fxIsArray($ha) && isset($ha['action']) && strlen($ha['action']))
		{
			// Prepare SQL array
			$sa=array();

			// ...names (insert/update)
			if(($ha['action'] == 'insert') || ($ha['action'] == 'update'))
			{
				$nid=$ha['nid'];
				if(!$nid)
					$nid=id_max('feiertage_name', 'feiertag_name');

				foreach($la as $lang => $dummy)
				{
					$where="feiertag_name=".$nid." AND id_sprache=".$lang." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$sql="SELECT f_tag, name_feiertag, feiertag_art FROM feiertage_name WHERE ".$where;
					$lha=db_values($sql);
					// Insert
					if(!fxIsArray($lha))
					{
						$np='';
						if($lang != $GLOBALS['fxpglobals']['lang'])
							$np='§';
						$sa[]=db_array2sql(array('mode'=>"insert", 'table'=>"feiertage_name", 'columns'=>array('mandanten_id'=>$GLOBALS['fxpglobals']['client'], 'feiertag_name'=>$nid, 'id_sprache'=>$lang, 'f_tag'=>$ha['calc'], 'name_feiertag'=>$np.$ha['name'], 'feiertag_art'=>$ha['type'])));
					}
					// Update
					else
					{
						$hn='';
						if($lang == $GLOBALS['fxpglobals']['lang'])
							$hn=$ha['name'];
						else if(mb_substr($lha['name_feiertag'],0,1) == '§')
							$hn='§'.$ha['name'];
						else if(($lha['f_tag'] != $ha['calc']) || ($lha['feiertag_art'] != $ha['type']))
							$hn=$lha['name_feiertag'];
						if(strlen($hn))
							$sa[]=db_array2sql(array('mode'=>"update", 'table'=>"feiertage_name", 'where'=>$where, 'columns'=>array('f_tag'=>$ha['calc'], 'name_feiertag'=>$np.$ha['name'], 'feiertag_art'=>$ha['type'])));
					}
				}
			}

			// ...dates/types
			$hid=$ha['hid'];
			if(!$hid && (($ha['action'] == 'insert') || ($ha['action'] == 'update')))
				$hid=id_max('feiertage_mand', 'feiertag_id');
			if($hid)
			{
				$where="feiertag_id=".$hid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				if($ha['action'] == 'delete')
				{
					$sa[]=db_array2sql(array('mode'=>"delete", 'table'=>"feiertage_mand", 'where'=>$where));
					if($ha['nid'])
						$na[$ha['nid']]=$hl;
				}
				else
				{
					$sql="SELECT datum, feiertag_name, halbtaegig FROM feiertage_mand WHERE ".$where;
					$hha=db_values($sql);
					// Insert
					if(!fxIsArray($hha))
						$sa[]=db_array2sql(array('mode'=>"insert", 'table'=>"feiertage_mand", 'columns'=>array('mandanten_id'=>$GLOBALS['fxpglobals']['client'], 'feiertag_id'=>$hid, 'datum'=>$ha['date'], 'feiertag_name'=>$nid, 'halbtaegig'=>$ha['length'])));
					// Update
					else if(($hha['datum'] != $ha['date']) || ($hha['feiertag_name'] != $nid) || ($hha['halbtaegig'] != $ha['length']))
						$sa[]=db_array2sql(array('mode'=>"update", 'table'=>"feiertage_mand", 'where'=>$where, 'columns'=>array('datum'=>$ha['date'], 'feiertag_name'=>$nid, 'halbtaegig'=>$ha['length'])));
				}
			}

			// Execute SQL's
			if(sizeof($sa))
			{
fxDebug($sa,'$sa', 3);
				$dberr=false;
				foreach($sa as $sql)
				{
					$dberr=db_x($sql);
fxDebug($dberr,'$dberr: $sql='.$sql, 2);
					if($dberr)
						break;
				}
//$dberr=true;
				if($dberr)
				{
					// An error occured while {inserting/updating/deleting} $var1!
					$err=true;
					error_msg(147+$GLOBALS['fxpglobals']['action'],false, '<br /><b class=red>'.$ha['name'].'</b> <i class=red>('.get_entry($ha['date'],'datum').')</i>');
				}
				else
				{
					$sc++;
					$sl .= '&rarr; <b class=green>'.$ha['name'].'</b> <i class=green>('.get_entry($ha['date'],'datum').')</i><br />';
				}
			}
		}
	}

	// Delete names if not in use anymore?
	if(!$err && sizeof($na))
	{
		foreach($na as $nid => $hl)
		{
			$where="feiertag_name=".$nid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$sql="SELECT feiertag_id FROM feiertage_mand WHERE ".$where;
			$hid=(int)db_value($sql);
fxDebug($hid,'$hid: $sql='.$sql, 2);
			if(!$hid)
			{
				$sql=db_array2sql(array('mode'=>"delete", 'table'=>"feiertage_name", 'where'=>$where));
				$dberr=db_x($sql);
fxDebug($dberr,'$dberr: $sql='.$sql, 2);
				// An error occured while {deleting} $var1!
				if($dberr)
				{
					$err=true;
					error_msg(151,false, '<br /><b class=red>'.$hva[$hl]['name'].'</b> <i class=red>('.get_entry($hva[$hl]['date'],'datum').')</i>');
				}
			}
		}
	}

	// Reset vacation day calculations for this year
	if(!$err && $sc && $year)
		$err=reset_resturlaub(0, $year);

	// Recalculate project utilization for this year
	if(!$err && $sc && $year)
	{
		$err=update_auslastung(0, $year.'0101', $year.'1231');

		// Error while calculating the project utilization!
		if($err)
			error_msg(846);
	}

	// Recalculate absence appointments for this year
	if(!$err && $sc && $year)
	{
		$err=update_appointments(0, $year);

		// Error adjusting the  <b class=red>Absence appointments</b>!$var1
		if($err)
			error_msg(3350,false, '<br /><br />'.$GLOBALS['_uama']['ko']);
		// The  <b class=green>Absence appointments</b> were successfully adjusted.$var1
		else if(strlen($GLOBALS['_uama']['ok']))
			error_msg(3349, false, '<br /><br />'.$GLOBALS['_uama']['ok']);
	}

	// Success message
	if(!$err)
	{
		// The following {Holidays} were successfully {created/changed/deleted}
		if($sc)
		{
			$ama=explode(',', meldung(14));
			error_msg(3340,false, '<b>'.get_text(343,'20',0,2).'</b>', $ama[$GLOBALS['fxpglobals']['action']-1].':<br />'.$sl);
		}
		// No changes were made.
		else
			error_msg(160);
	}
//$err=true;
}
?>