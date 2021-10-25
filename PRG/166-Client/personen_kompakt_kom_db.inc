<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : personen_kompakt_kom_db.inc                                  //
// Version     : 21.2                                                         //
// Begin       : 2020-10-06                                                   //
// Last Change : 2020-10-06                                                   //
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
 * PF 166: Client etc. - Communication database action preparations (will be called as an alternative program function from other person types as well)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

// Variable preparations
$db_noerror_old=$db_noerror;
$db_noerror=true;

$dbz_arr['kom']=array();
$dbz=0;

$akey=-1;
if(fxIsArray($_POST) && fxIsArray($_POST['Kommunikation_ID']))
{
	reset($_POST['Kommunikation_ID']);
	$akey=key($_POST['Kommunikation_ID']);
}

// Cycle through all communications
if(($akey >= 0) && fxIsArray($_POST['Kommunikation_ID'][$akey]))
{
	foreach($_POST['Kommunikation_ID'][$akey] as $komcnt => $kval)
	{
		// Empty line?
		if(!strlen($kval))
			continue;

		$kval=(int)$kval;

		// Create or Change
		if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
		{
			if(($kval < 0) && !$_POST['KommunikationsTyp'][$akey][$komcnt] && !strlen($_POST['Kommunikation_txt'][$akey][$komcnt]) && !strlen($_POST['Anmerkung'][$akey][$komcnt]))
				continue;

			$db_KommunikationsTyp=$_POST['KommunikationsTyp'][$akey][$komcnt];
			$db_Kommunikation_txt="'".convert_string($_POST['Kommunikation_txt'][$akey][$komcnt], 'todb')."'";
			if(strlen($_POST['Anmerkung'][$akey][$komcnt]))
				$db_Anmerkung="'".convert_string($_POST['Anmerkung'][$akey][$komcnt], 'todb')."'";
			else
				$db_Anmerkung="NULL";

			// Create
			if($kval < 0)
			{
				$nkomid=id_max('kommunikation', 'kommunikation_id');
				$sql  = "INSERT INTO kommunikation (mandanten_id, kommunikation_id, personen_id, kommunikationstyp, kommunikation_txt, anmerkung)";
				$sql .= " VALUES (".$_cid.", ".$nkomid.", ".$_pid.", ".$db_KommunikationsTyp.", ".$db_Kommunikation_txt.", ".$db_Anmerkung.")";
				$dbz_arr['kom'][$dbz++]=array('sql'=>$sql, 'typ'=>'insert');
			}
			// Change (only if data has changed)
			else
			{
				$sql="SELECT kommunikationstyp, kommunikation_txt, anmerkung FROM kommunikation WHERE kommunikation_id=".$kval." AND mandanten_id=".$_cid;
				$chk=db_values($sql);
				if(fxIsArray($chk))
				{
					$dif=false;
					$car=array('iKommunikationsTyp', 'tKommunikation_txt', 'tAnmerkung');
					foreach($car as $ctn)
					{
						$ctp=substr($ctn, 0, 1);
						$cnn=substr($ctn, 1);
						$cnl=strtolower($cnn);
						$cov=${'db_'.$cnn};
						if($ctp == 'i')	// Integer
						{
							if($chk[$cnl] != $cov)
							{
								$dif=true;
								break;
							}
						}
						else			// Text
						{
							if(strlen($chk[$cnl]))
								$cmp="'".convert_string($chk[$cnl], 'todb')."'";
							else
								$cmp="NULL";
							if($cmp != $cov)
							{
								$dif=true;
								break;
							}
						}
					}
				}
				else
					$dif=true;

				if($dif)
				{
					$sql="UPDATE kommunikation SET kommunikationstyp=".$db_KommunikationsTyp.", kommunikation_txt=".$db_Kommunikation_txt.", anmerkung=".$db_Anmerkung." WHERE kommunikation_id=".$kval." AND mandanten_id=".$_cid;
					$dbz_arr['kom'][$dbz++]=array('sql'=>$sql, 'typ'=>'update');
				}
			}
		}
		// Delete (if checkbox is ticked)
		else if($_POST['Button_PZLoeschen'] && $_POST['checkbox'][$akey][$komcnt])
		{
			$sql="DELETE FROM kommunikation WHERE kommunikation_id=".$kval." AND mandanten_id=".$_cid;
			$dbz_arr['kom'][$dbz++]=array('sql'=>$sql, 'typ'=>'delete');
		}
	}
}

// Reset error messages
if(!$db_noerror_old)
	unset($db_noerror);
?>