<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : personen_kompakt_adr_db.inc                                  //
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
 * PF 166: Client etc. - Address database action preparations (will be called as an alternative program function from other person types as well)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

// Variable preparations
$db_noerror_old=$db_noerror;
$db_noerror=true;

$dbz_arr['adr']=array();
$dbz=0;

$akey=-1;
if(fxIsArray($_POST) && fxIsArray($_POST['Adressen_ID']))
{
	reset($_POST['Adressen_ID']);
	$akey=key($_POST['Adressen_ID']);
}

// Cycle through all addresses
if(($akey >= 0) && fxIsArray($_POST['Adressen_ID'][$akey]))
{
	foreach($_POST['Adressen_ID'][$akey] as $adcnt => $aval)
	{
		// Empty line?
		if(!strlen($aval))
			continue;

		$aval=(int)$aval;

		// Create or Change
		if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
		{
			if(($aval < 0) && !$_POST['AdressTyp'][$akey][$adcnt] && !$_POST['LandKZ'][$akey][$adcnt] && !strlen($_POST['Staat'][$akey][$adcnt]) && !strlen($_POST['Strasse'][$akey][$adcnt]) && !strlen($_POST['PLZ'][$akey][$adcnt]) && !strlen($_POST['Ort'][$akey][$adcnt]))
				continue;

			$db_AdressTyp=$_POST['AdressTyp'][$akey][$adcnt];
			$db_LandKZ=$_POST['LandKZ'][$akey][$adcnt];
			$db_Strasse="'".convert_string($_POST['Strasse'][$akey][$adcnt], 'todb')."'";
			$db_PLZ="'".convert_string($_POST['PLZ'][$akey][$adcnt], 'todb')."'";
			$db_Ort="'".convert_string($_POST['Ort'][$akey][$adcnt], 'todb')."'";
			if(strlen($_POST['Staat'][$akey][$adcnt]))
				$db_Staat="'".convert_string($_POST['Staat'][$akey][$adcnt], 'todb')."'";
			else
				$db_Staat="NULL";

			// Create
			if($aval < 0)
			{
				$nadid=id_max('adressen', 'adressen_id');
				$sql  = "INSERT INTO adressen (mandanten_id, adressen_id, personen_id, strasse, plz, ort, staat, landkz, adresstyp)";
				$sql .= " VALUES (".$_cid.", ".$nadid.", ".$_pid.", ".$db_Strasse.", ".$db_PLZ.", ".$db_Ort.", ".$db_Staat.", ".$db_LandKZ.", ".$db_AdressTyp.")";
				$dbz_arr['adr'][$dbz++]=array('sql'=>$sql, 'typ'=>'insert');
			}
			// Change (only if data has changed)
			else
			{
				$sql="SELECT strasse, plz, ort, staat, landkz, adresstyp FROM adressen WHERE adressen_id=".$aval." AND mandanten_id=".$_cid;
				$chk=db_values($sql);
				if(fxIsArray($chk))
				{
					$dif=false;
					$car=array('iAdressTyp', 'tStrasse', 'tStaat', 'iLandKZ', 'tPLZ', 'tOrt');
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
					$sql="UPDATE adressen SET strasse=".$db_Strasse.", plz=".$db_PLZ.", ort=".$db_Ort.", staat=".$db_Staat.", landkz=".$db_LandKZ.", adresstyp=".$db_AdressTyp." WHERE adressen_id=".$aval." AND mandanten_id=".$_cid;
					$dbz_arr['adr'][$dbz++]=array('sql'=>$sql, 'typ'=>'update');
				}
			}
		}
		// Delete (if checkbox is ticked)
		else if($_POST['Button_PZLoeschen'] && $_POST['checkbox'][$akey][$adcnt])
		{
			$sql="DELETE FROM adressen WHERE adressen_id=".$aval." AND mandanten_id=".$_cid;
			$dbz_arr['adr'][$dbz++]=array('sql'=>$sql, 'ckd'=>array('tab'=>'Adressen', 'fld'=>'Adressen_ID', 'val'=>$aval), 'typ'=>'delete');
		}
	}
}

// Reset error messages
if(!$db_noerror_old)
	unset($db_noerror);
?>