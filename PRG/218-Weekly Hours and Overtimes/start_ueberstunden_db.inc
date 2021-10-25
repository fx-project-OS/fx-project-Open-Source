<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : start_ueberstunden_db.inc                                    //
// Version     : 21.2                                                         //
// Begin       : 2020-11-09                                                   //
// Last Change : 2020-11-09                                                   //
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
 * PF 218: Weekly Hours and Overtimes
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$dbg218=0;
if($disp_list && !$err && fxIsArray($_POST) && $_POST['Button_Submit'])
{
	$chg=0;
	$chb=0;
	$cht=0;
	$aam='';

	// Monatüberträge löschen
	if(!$err && fxIsArray($cba))
	{
		foreach($cba as $pid => $cd)
		{
			if(!fxIsArray($cha[$pid]))
			{
				$sql="DELETE FROM zeitdaten WHERE personen_id=".$pid." AND zeitart=104 AND ist_beginn>='".$cd."00000000' AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$err=db_x($sql);
if($dbg218) fxDebug($err, $sql, 0);
				if($err)
					error_msg(163, false);	// 163: Fehler beim Löschen der alten Übertrags-Startwerte bzw. -Zwischenbilanzen!
				else
					$chb++;
			}
		}
	}

	// Übertrags-Startwerte setzen
	if(!$err && fxIsArray($cha))
	{
		$pids='';
		if(fxIsArray($cha))
			$pids=array_to_csv($cha);
//echo('$pids='.$pids.'<br />');

		// Alte Zwischenbilanzen und Startwerte immer löschen
		$sql="DELETE FROM zeitdaten WHERE personen_id IN (".$pids.") AND zeitart IN (104,105) AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$err=db_x($sql);
if($dbg218) fxDebug($err, $sql, 0);
		if($err)
			error_msg(163, false);	// 163: Fehler beim Löschen der alten Übertrags-Startwerte bzw. -Zwischenbilanzen!
		else
		{
			// Temporären INSERT
			$tsql  = "INSERT INTO zeitdaten";
			$tsql .= " (mandanten_id, zuordnungs_mid, zeit_id, personen_id, ist_beginn, ist_ende, zeitaufwand, rk_ort1, zeitart) ";
			$tsql .= "VALUES";
			$tsql .= " (".$GLOBALS['fxpglobals']['client'].", ".$GLOBALS['fxpglobals']['client'].", {tid}, {pid}, {dat}, {dat}, {val}, {not}, 105)";

			// Neue Wochenarbeitsstunden und Übertragsstartwerte für jede Person setzen
			foreach($cha as $pid => $arr)
			{
				$val_zeilennummer=$arr['line'];

				// Neue Wochenarbeitsstunden setzen?
				if(array_key_exists('shours', $arr))
				{
					$sql="UPDATE madaten SET wochenarbeitstd=".$arr['shours']." WHERE personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$err=db_x($sql);
if($dbg218) fxDebug($err, $sql, 0);
					if($err)
					{
						error_msg(164, array('Ueberstunden_rest'));	// 164: Fehler beim Anlegen der neuen Übertrags-Startwerte!
						break;
					}

					$err=update_auslastung($pid);
					if($err)
					{
						error_msg(846, array('WochenArbeitStd'));	// 846: Fehler bei der Projektauslastungsberechnung!
						break;
					}
				}

				// Neuen Übertragsstartwert setzen
				$varr=array(
					'{tid}'	=> id_max('zeitdaten', 'zeit_id'),
					'{pid}'	=> $pid,
					'{dat}'	=> $arr['date'],
					'{val}'	=> $arr['value'],
					'{not}'	=> $arr['note']
				);
				$sql=strtr($tsql, $varr);
				$err=db_x($sql);
if($dbg218) fxDebug($err, $sql, 0);

				if($err)
				{
					error_msg(164, array('Ueberstunden_rest'));	// 164: Fehler beim Anlegen der neuen Übertrags-Startwerte!
					break;
				}
				else
				{
					$caa[$pid]=1969;
					$chg++;
				}
			}
		}
	}

	// Abwesenheitstermine anpassen
	if(!$err && fxIsArray($caa))
	{
		foreach($caa as $pid => $cd)
		{
			$err=update_appointments($pid);
			if($err)
			{
				error_msg(3350, array('WochenArbeitStd','Monat','Jahr'), '<br /><br />'.$GLOBALS['_uama']['ko']);	// 3350: Fehler beim Anpassen der Abwesenheitstermine!$var1
				break;
			}
			else
			{
				if(strlen($GLOBALS['_uama']['ok']))
				{
					if(strlen($aam))
						$aam .= '<br /><br />';
					$aam .= $GLOBALS['_uama']['ok'];
				}

				$cht++;
			}
		}
	}

	if(!$err)
	{
		if($dbg218 || (!$chg && !$chb && !$cht))
		{
			$err=true;
			error_msg(160,false);	// Es wurden keine Änderungen durchgeführt.
		}
		else if($chg)
		{
			if(strlen($aam))
				error_msg(3349, false, '<br /><br />'.$aam);	// 3349: Die Abwesenheitstermine wurden erfolgreich angepasst.$var1
			error_msg(165,false);	// 165: Die neuen Übertrags-Startwerte wurden erfolgreich in die jeweiligen Zeitkonten gesetzt.
		}
		else if($chb)
		{
			if(strlen($aam))
				error_msg(3349, false, '<br /><br />'.$aam);	// 3349: Die Abwesenheitstermine wurden erfolgreich angepasst.$var1
			error_msg(3130,false);	// 3130: Die Überträge wurden erfolgreich neu berechnet.
		}
		else
			error_msg(3349, false, '<br /><br />'.$aam);	// 3349: Die Abwesenheitstermine wurden erfolgreich angepasst.$var1
	}
//$err=true;
}
?>