<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : faefaehigkeiten_db.inc                                       //
// Version     : 24.1                                                         //
// Begin       : 2020-10-19                                                   //
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
 * PF 17: Skills
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */
if(!$err && $MaArt_ID && fxIsArray($_POST) && fxIsArray($_POST['MaFaehigkeit_ID']) && isset($_POST['Button_Multi']))
{
//fxDebug('_POST');

	reset($_POST['MaFaehigkeit_ID']);
	$mc=key($_POST['MaFaehigkeit_ID']);

	$la=fxGetAllLanguages();

	$ins=0;
	$upd=0;
	$del=0;
	$ins_val='';
	$upd_val='';
	$del_val='';
	foreach($_POST['MaFaehigkeit_ID'][$mc] as $hn => $id)
	{
		$id=(int)$id;
		if(!$id)
			continue;

		// Delete?
		if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
		{
			if(($id > 0) && fxIsArray($_POST['Checkbox']) && fxIsArray($_POST['Checkbox'][$mc]) && $_POST['Checkbox'][$mc][$hn])
			{
				$del_val=$_POST['Dummy'][$mc][$hn];

				$sql="DELETE FROM ma_faehigkeit WHERE mafaehigkeit_id=".$id." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$err=db_check_delete('ma_faehigkeit', 'mafaehigkeit_id', $id, $sql);
fxDebug($err,$sql, 2);
				if(!$err)
					$del++;
				else
					break;
			}
		}
		else if(fxIsArray($la) && fxIsArray($_POST['MaFaehigkeit_Name']) && fxIsArray($_POST['MaFaehigkeit_Name'][$mc]) && isset($_POST['MaFaehigkeit_Name'][$mc][$hn]) && !is_null($_POST['MaFaehigkeit_Name'][$mc][$hn]) && strlen((string)$_POST['MaFaehigkeit_Name'][$mc][$hn]))
		{
			$sql="SELECT id_sprache, mafaehigkeit_name FROM ma_faehigkeit WHERE mafaehigkeit_id=".$id." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$tmp=db_values($sql,'*');
			$gla=array();
			if(fxIsArray($tmp))
			{
				foreach($tmp as $t)
					$gla[(int)$t['id_sprache']]=$t['mafaehigkeit_name'];
			}
fxDebug($gla,'$gla', 3);

			if($id < 0)
				$id=id_max('ma_faehigkeit', 'mafaehigkeit_id', '', '', 100);

			$uc=0;
			foreach($la as $lid => $a)
			{
				$vp='';
				if($lid != $GLOBALS['fxpglobals']['lang'])
					$vp='§';
				$ov='';
				if(isset($gla[$lid]))
					$ov=$gla[$lid];
				// Update?
				if(isset($gla[$lid]))
				{
					if(($_POST['MaFaehigkeit_Name'][$mc][$hn] != $_POST['Dummy'][$mc][$hn]) && (!mb_strlen((string)$vp) || (mb_substr((string)$gla[$lid],0,1) == $vp)))
					{
						$uc++;
						$sql  = "UPDATE ma_faehigkeit SET mafaehigkeit_name='".convert_string($vp.$MaFaehigkeit_Name[$mc][$hn], 'todb')."'";
						$sql .= " WHERE mafaehigkeit_id=".$id." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND id_sprache=".$lid;
						$err=db_x($sql, 0);
fxDebug($err,$sql, 2);
						if($err)
							break;
					}
				}
				// Insert?
				else
				{
					$uc++;
					$sql  = "INSERT INTO ma_faehigkeit (mandanten_id, mafaehigkeit_id, id_sprache, maart_id, mafaehigkeit_name)";
					$sql .= " VALUES (".$GLOBALS['fxpglobals']['client'].", ".$id.", ".$lid.", ".$MaArt_ID.", '".convert_string($vp.$_POST['MaFaehigkeit_Name'][$mc][$hn], 'todb')."')";
					$err=db_x($sql, 0);
fxDebug($err,$sql, 2);
					if($err)
						break;
				}
			}

			if(sizeof($gla))
			{
				if($uc)
				{
					$upd_val=$_POST['MaFaehigkeit_Name'][$mc][$hn];
					// An error occurred while changing the data record "$var1"!
					if($err)
						error_msg(1522, false, $upd_val);
					else
						$upd++;
				}
			}
			else
			{
				$ins_val=$_POST['MaFaehigkeit_Name'][$mc][$hn];
				// An error occurred while creating the data record "$var1"!
				if($err)
					error_msg(1521, false, $ins_val);
				else
					$ins++;
			}
		}

		if($err)
			break;
	}

	// Success messages
	if(!$err)
	{
		if($ins || $upd || $del)
		{
			// The data record "$var1" was deleted succesfully.
			if($del == 1)
				error_msg(1517, false, '<b>'.$del_val.'</b>');
			// $var1 data records were successfully deleted.
			else if($del)
				error_msg(116, false, $del);

			// The data record "$var1" was changed succesfully.
			if($upd == 1)
				error_msg(1516, false, '<b>'.$upd_val.'</b>');
			// $var1 data records were successfully changed.
			else if($upd)
				error_msg(115, false, $upd);

			// The data record "$var1" was created succesfully.
			if($ins == 1)
				error_msg(1515, false, '<b>'.$ins_val.'</b>');
			// $var1 data records were successfully created.
			else if($ins)
				error_msg(114, false, $ins);
		}
		// There were no entries ticked!
		else if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
			error_msg(1642);
		// No changes were made.
		else
			error_msg(160);
	}
//$err=true;
}
?>