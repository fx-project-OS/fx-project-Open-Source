<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : ekvk_eingabe_val.inc                                         //
// Version     : 21.2                                                         //
// Begin       : 2020-11-17                                                   //
// Last Change : 2021-10-06                                                   //
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
 * 268: Hourly Rates
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

if(fxIsArray($_POST) && isset($_POST['Button_Speichern']))
{
//fxDebug('_POST');

	// Arrays definieren zum Speichern der Aktionen
	$new_array=array();
	$upd_array=array();
	$del_array=array();
	$err_array=array();

	// Standardvariablen
	$mc='';
	if(fxIsArray($_POST['ID']))
	{
		reset($_POST['ID']);
		$mc=key($_POST['ID']);
	}

	if(strlen($mc) && fxIsArray($_POST['ID'][$mc]))
	{
		$id=(int)$_POST['ID'][$mc][0];
		$skat=(int)$_POST['Kategorie'];
		$kat=(int)$_POST['Feldtyp'][$mc][0];

		$pia=explode('_', $_POST['AuswahlFeldtyp']);
		if(sizeof($pia) == 1)
			$sid=(int)$pia[0];
		else
			$sid=(int)$pia[1];

		// Liste durchgehen
		foreach($_POST['ID'][$mc] as $val_zeilennummer => $hid)
		{
			$pk=(int)$_POST['Ma_Art_Faehigkeit_ID'][$mc][$val_zeilennummer].'|'.(int)$_POST['MaFaehigkeit_ID'][$mc][$val_zeilennummer];
			if(!isset($err_array[$pk]))
			{
				$err_array[$pk]=array("ek"=>'^'.$_POST['EK'][$mc][$val_zeilennummer], "vk"=>'^'.$_POST['VK'][$mc][$val_zeilennummer], "cb"=>$_POST['Loeschen'][$mc][$val_zeilennummer]);
//fxDebug($err_array[$pk],'$err_array['.$pk.']');
				// Stimmt Auswahlmaskenkriterien noch mit Liste übereinstimmt, dann Validierung
				if(($skat == $kat) && (!$id || ($sid == $id)))
				{
					$hid=(int)$hid;
					$hkat=(int)$_POST['Feldtyp'][$mc][$val_zeilennummer];
					$ek=trim($_POST['EK'][$mc][$val_zeilennummer]);
					$vk=trim($_POST['VK'][$mc][$val_zeilennummer]);
					// ...Eintrag löschen
					if((!strlen($ek) && !strlen($vk)) || $_POST['Loeschen'][$mc][$val_zeilennummer])
					{
						if(strlen($_POST['nv_text1'][$mc][$val_zeilennummer]) && (substr($_POST['nv_text1'][$mc][$val_zeilennummer],0,1) != '~'))
							$del_array[$pk]=array("line"=>$val_zeilennummer, "feldtyp"=>$hkat, "id"=>$hid, "maart_id"=>(int)$_POST['Ma_Art_Faehigkeit_ID'][$mc][$val_zeilennummer], "mafaehigkeit_id"=>(int)$_POST['MaFaehigkeit_ID'][$mc][$val_zeilennummer], "ek"=>$_POST['nv_betrag1'][$mc][$val_zeilennummer], "vk"=>$_POST['nv_betrag2'][$mc][$val_zeilennummer]);
					}
					// ...Validierung
					else
					{
						$verr_ek=false;
						$vek='NULL';
						if(strlen($ek))
						{
							$vek=validate_entry($ek, $verr_ek, 'dezimal', '1');
							if($verr_ek)
							{
								error_msg($verr_ek, -1, 'EK');
								$err=true;
							}
							else
								$err_array[$pk]['ek']=$vek;
						}

						$verr_vk=false;
						$vvk='NULL';
						if(strlen($vk))
						{
							$vvk=validate_entry($vk, $verr_vk, 'dezimal', '1');
							if($verr_vk)
							{
								error_msg($verr_vk, -1, 'VK');
								$err=true;
							}
							else
								$err_array[$pk]['vk']=$vvk;
						}

						// Validierung ok, dann Neuanlage oder Ändern
						if(!$verr_ek && !$verr_vk)
						{
							$oek=$_POST['nv_betrag1'][$mc][$val_zeilennummer];
							if(!strlen($oek))
								$oek='NULL';
							else
								$oek=get_double($oek);

							$ovk=$_POST['nv_betrag2'][$mc][$val_zeilennummer];
							if(!strlen($ovk))
								$ovk='NULL';
							else
								$ovk=get_double($ovk);

							// ...Nur, falsch unterschiedlich zu alten Werten
							if(($vek != $oek) || ($vvk != $ovk))
							{
								$arr=array("line"=>$val_zeilennummer, "feldtyp"=>$hkat, "id"=>$hid, "maart_id"=>(int)$_POST['Ma_Art_Faehigkeit_ID'][$mc][$val_zeilennummer], "mafaehigkeit_id"=>(int)$_POST['MaFaehigkeit_ID'][$mc][$val_zeilennummer], "ek"=>$vek, "vk"=>$vvk);
								// ...Ändern
								if($_POST['nv_text1'][$mc][$val_zeilennummer])
									$upd_array[$pk]=$arr;
								// ...Neuanlage
								else
									$new_array[$pk]=$arr;
							}
						}
					}
				}
			}
		}

		// Stimmt Auswahlmaskenkriterien nicht mehr mit Liste übereinstimmt, dann Auswahl zurücksetzen und Hinweis
		if(($skat != $kat) || ($id && ($sid != $id)))
		{
			$_POST['Kategorie']=$kat;
			$_POST['AuswahlFeldtyp']=$id;

			if($kat == 21)	// Personen
				$_POST['AuswahlFeldtyp']=$GLOBALS['fxpglobals']['client'].'_'.$_POST['AuswahlFeldtyp'];

			// 1933: Die Auswahlkriterien stimmten nicht mehr mit den angezeigten Datensätzen überein! Die Auswahlkriterien wurden deshalb zurückgesetzt. Bitte klicken Sie evtl. erneut auf den Aktionsbutton um die Datensätze zu speichern.
			error_msg(1933);
			$err=true;
		}
	}

	// Hinweis, falls nichts verändert werden muss
	if(!$err && !sizeof($new_array) && !sizeof($upd_array) && !sizeof($del_array))
	{
		// 113: Es müssen keine Datensätze geändert oder gelöscht werden.
		error_msg(113);
		$err=true;
	}

	if($err)
		$db_update=false;
}
else
	$db_update=false;
//$err=true;
?>