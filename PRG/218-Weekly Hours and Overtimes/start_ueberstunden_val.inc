<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : start_ueberstunden_val.inc                                   //
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

$dbgpid=0;
if($dbgpid > 0) fxDebug('_POST');

if($disp_list && !$err && fxIsArray($_POST) && fxIsArray($_POST['Personen_ID']) && $_POST['Button_Submit'])
{
	// Key des übergebenen Arrays ermitteln
	reset($_POST['Personen_ID']);
	$akey=key($_POST['Personen_ID']);

	// Validierung
	$cha=array();
	$cba=array();
	$caa=array();
	if(fxIsArray($_POST['Personen_ID'][$akey]))
	{
		foreach($_POST['Personen_ID'][$akey] as $pcnt => $pid)
		{
			$hid=explode('^', $_POST['nv_text2'][$akey][$pcnt]);
			$pid=(int)$pid;
			$cmp='';
if(($dbgpid > 0) && ($pid == $dbgpid)) fxDebug($hid,'$hid: $pid='.$pid, 0);

			$val_zeilennummer=$pcnt;

			if($pid)
			{
				// Std./Woche
				$verr=false;
				$val=trim($_POST['WochenArbeitStd'][$akey][$pcnt]);
				$_POST['WochenArbeitStd'][$akey][$pcnt]=validate_entry($val, $verr, 'dezimal', '1');
if(($dbgpid > 0) && ($pid == $dbgpid)) fxDebug($_POST['WochenArbeitStd'][$akey][$pcnt], 'WochenArbeitStd: '.$val, 0);
				if($verr)
				{
					$err=true;
					error_msg($verr, false, 'WochenArbeitStd');
				}
				else if($_POST['WochenArbeitStd'][$akey][$pcnt] > $GLOBALS['fxpglobals']['settings']['maxstd_woche'])
				{
					$err=true;
					// 2439: Ein Mitarbeiter darf pro Woche maximal $var1 Stunden arbeiten.
					error_msg(2439, array('WochenArbeitStd'), get_entry($GLOBALS['fxpglobals']['settings']['maxstd_woche'], 'dezimal'));
				}
				else if(($_POST['WochenArbeitStd'][$akey][$pcnt] / $hid[1]) > $GLOBALS['fxpglobals']['settings']['maxstd_tag'])
				{
					$err=true;
					// 2438: Ein Mitarbeiter darf pro Tag maximal $var1 Stunden arbeiten. Dieser Mitarbeiter müsste aber nach Ihren Angaben täglich $var2 Stunden leisten.
					error_msg(2438, array('WochenArbeitStd'), get_entry($GLOBALS['fxpglobals']['settings']['maxstd_tag'], 'dezimal'), get_entry($_POST['WochenArbeitStd'][$akey][$pcnt]/$hid[1], 'dezimal'));
				}
				else
					$val=get_entry($_POST['WochenArbeitStd'][$akey][$pcnt], 'dezimal');
				$cmp=$val;

				// Monat
				$val=(int)$_POST['Monat'][$akey][$pcnt];
if(($dbgpid > 0) && ($pid == $dbgpid)) fxDebug($_POST['Monat'][$akey][$pcnt], 'Monat: '.$val, 0);
				$cmp .= '|'.$val;

				// Jahr
				$verr=false;
				$val=trim($_POST['Jahr'][$akey][$pcnt]);
				$_POST['Jahr'][$akey][$pcnt]=validate_entry($val, $verr, 'ganzzahl', '1');
if(($dbgpid > 0) && ($pid == $dbgpid)) fxDebug($_POST['Jahr'][$akey][$pcnt], 'Jahr: '.$val, 0);
				if($verr)
				{
					$err=true;
					error_msg($verr, false, 'Jahr');
				}
				else if(strlen($_POST['Jahr'][$akey][$pcnt]) != 4)
				{
					$err=true;
					// 162: Bitte geben Sie in das Feld "$var1" eine Jahreszahl ein!
					error_msg(162, false, 'Jahr');
				}
				else
					$val=$_POST['Jahr'][$akey][$pcnt];
				$cmp .= '|'.$val;

				// Rest-Überstunden
				$verr=false;
				$val=trim($_POST['Ueberstunden_rest'][$akey][$pcnt]);
				$_POST['Ueberstunden_rest'][$akey][$pcnt]=validate_entry($val, $verr, 'zeitspanne');
				if(!$verr && !strlen($_POST['Ueberstunden_rest'][$akey][$pcnt]))
					$_POST['Ueberstunden_rest'][$akey][$pcnt]=0;
if(($dbgpid > 0) && ($pid == $dbgpid)) fxDebug($_POST['Ueberstunden_rest'][$akey][$pcnt], 'Ueberstunden_rest: '.$val, 0);
				if($verr)
				{
					$err=true;
					error_msg($verr, false, 'Ueberstunden_rest');
				}
				else
					$val=$_POST['Ueberstunden_rest'][$akey][$pcnt];
				$cmp .= '|'.$val;

				// Anmerkung
				$note=trim($_POST['Anmerkung'][$akey][$pcnt]);
				$cmp .= '|'.$note;
if(($dbgpid > 0) && ($pid == $dbgpid)) fxDebug($cmp,'$cmp: $pid='.$pid, 0);

				// Wurde eine Änderung durchgeführt?
				if($cmp != $hid[0])
				{
					$year=$_POST['Jahr'][$akey][$pcnt];
					$month=$_POST['Monat'][$akey][$pcnt]-18;
					if($month < 1)
					{
						$year--;
						$month=12;
					}
					if(!strlen($note))
						$note='NULL';
					else
						$note="'".$note."'";

					$cha[$pid]=array('line'=>$pcnt, 'date'=>"'".$year.substr('00'.$month, -2)."00000000'", 'value'=>$_POST['Ueberstunden_rest'][$akey][$pcnt], 'note'=>$note);

					// Wurden die Wochenarbeitsstunden geändert?
					$ca=explode('|', $cmp);
					$ha=explode('|', $hid[0]);
					if($ca[0] != $ha[0])
						$cha[$pid]['shours']=$_POST['WochenArbeitStd'][$akey][$pcnt];
				}

				// Monatsübertrag ändern?
				if(strlen($hid[2]))
					$cba[$pid]=$hid[2];

				// Abwesenheitstermine ändern?
				if((int)$hid[3] > 0)
					$caa[$pid]=(int)$hid[3];
			}
		}
if($dbgpid > 0) {fxDebug($cha, '$cha', 0); fxDebug($cba, '$cba', 0); fxDebug($caa, '$caa', 0);}
	}
//$err=true;
}
?>