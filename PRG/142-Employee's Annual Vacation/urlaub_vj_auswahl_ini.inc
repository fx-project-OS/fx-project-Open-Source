<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : urlaub_vj_auswahl_ini.inc                                    //
// Version     : 21.2                                                         //
// Begin       : 2020-10-21                                                   //
// Last Change : 2020-10-21                                                   //
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
 * PF 142: Employee's Annual Vacation
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

// Keine Indizierung
if(isset($index))
	unset($index);

// Auswahlmaske (124) immer im Änderungsmodus + Datumsfelder inkl. Popup-Icon
$maske124_sf=true;
$GLOBALS['cpo'.$tr]=true;

// Personenauswahlfelder
$va=array('Suchtext'=>"t", 'Zeitspanne_von'=>"d", 'Zeitspanne_bis'=>"d", 'Abt_ID'=>'s0', 'akt_ma'=>"c1", 'historie'=>"c0");
fxDebug($va,'$va', 3);
foreach($va as $sv => $vv)
{
	$svl=strtolower($sv);
	$vvt=substr($vv,0,1);
	$vvd=substr($vv,1);
	// Defaultwert setzen
	if(($vvt == 's') || ($vvt == 'c'))	// Typ "s" (= Select/Combobox) oder "c" (= Checkbox) erwartet Ganzzahl
		$GLOBALS[$svl]=(int)$vvd;
	else
		$GLOBALS[$svl]=$vvd;
//echo('Default-Wert für <b>'.$sv.'</b>: $'.$svl.' = ['.$GLOBALS[$svl].']<br />');

	// Validierung der Personenauswahl
	if(fxIsArray($_POST))
	{
		if(isset($_POST[$sv]))
		{
			$GLOBALS[$svl]=trim($_POST[$sv]);
			if(($vvt == 's') || ($vvt == 'c'))	// Typ "s" (= Select/Combobox) oder "c" (= Checkbox) erwartet Ganzzahl
				$GLOBALS[$svl]=(int)$GLOBALS[$svl];
			else if($vvt == 'd')	// Datumsfeld gesondert validieren
			{
				$v=validate_entry($GLOBALS[$svl], $verr, 'datum');
				// 521: Bitte geben Sie im Feld $var1 ein gültiges Datum ein.
				if($verr)
				{
					error_msg(521, false, $sv);
					$err=true;
				}
				else
					$v=substr($v,0,8);
				$GLOBALS[$svl]=$v;
			}
		}
		else if($vvt == 'c')
			$GLOBALS[$svl]="0";
		$_POST[$sv]=$GLOBALS[$svl];
//echo('POST-Wert für <b>'.$sv.'</b>: $'.$svl.' = ['.$GLOBALS[$svl].']<br />');
	}

	// Felder in Auswahlmaske setzen
	$GLOBALS['maskenfeldwerte'][$svl]=$GLOBALS[$svl];
}

// Keine Validierungsfehler, dann Prüfen, ob Vondatum hinter Bisdatum liegt - wenn JA, korrigieren
if(!$err && strlen($GLOBALS['zeitspanne_von']) && strlen($GLOBALS['zeitspanne_bis']) && ((int)$GLOBALS['zeitspanne_von'] > (int)$GLOBALS['zeitspanne_bis']))
{
	$swap=$GLOBALS['zeitspanne_von'];
	$GLOBALS['zeitspanne_von']=$GLOBALS['zeitspanne_bis'];
	$GLOBALS['zeitspanne_bis']=$swap;

	$GLOBALS['maskenfeldwerte']['zeitspanne_von']=$GLOBALS['zeitspanne_von'];
	$GLOBALS['maskenfeldwerte']['zeitspanne_bis']=$GLOBALS['zeitspanne_bis'];

	$_POST['Zeitspanne_von']=$GLOBALS['zeitspanne_von'];
	$_POST['Zeitspanne_bis']=$GLOBALS['zeitspanne_bis'];
}
//$err=true;

// Variable, ob Personenliste angezeigt werden soll oder nicht
$disp_list=false;
if(!$err && fxIsArray($_POST) && ($_POST['Button_Anzeigen'] || $_POST['Button_Multi']))
	$disp_list=true;
?>