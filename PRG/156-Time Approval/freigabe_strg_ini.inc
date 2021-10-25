<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : freigabe_strg_ini.inc                                        //
// Version     : 21.2                                                         //
// Begin       : 2020-11-12                                                   //
// Last Change : 2020-11-12                                                   //
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
 * PF 156: Time Approval
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

// Zeitspanne von + Zeitspanne bis [Datum]
if(fxIsArray($_GET) && isset($_GET['zvon']))	// Aus Navigation
{
	$Zeitspanne_von=substr($_GET['zvon'],0,8).'000000';
	if(isset($_GET['zbis']))
		$Zeitspanne_bis=substr($_GET['zbis'],0,8).'240000';
	else
		$Zeitspanne_bis=ultimo($_GET['zvon'],false).'240000';

	$_POST['Button_Anzeigen']=true;
	$Button_Anzeigen=true;
}
else
{
	$Zeitspanne_von='';
	if(fxIsArray($_POST) && isset($_POST['Zeitspanne_von']))
		$Zeitspanne_von=trim($_POST['Zeitspanne_von']);
	$Zeitspanne_bis='';
	if(fxIsArray($_POST) && isset($_POST['Zeitspanne_bis']))
		$Zeitspanne_bis=trim($_POST['Zeitspanne_bis']);
	if(!$Zeitspanne_von || !$Zeitspanne_bis)
	{
		$y=(int)substr($GLOBALS['datetime'],0,4);
		$m=(int)substr($GLOBALS['datetime'],4,2);
		$d=(int)substr($GLOBALS['datetime'],6,2);
		if($d < $GLOBALS['fxpglobals']['settings']['monatswechsel'])	// Letzter Monat
		{
			--$m;
			if(!$m)
			{
				--$y;
				$m=12;
			}
		}
		$von=makeDate($y,$m,1, 0);
		$bis=ultimo($von,false).'240000';
	}
	if(!$Zeitspanne_von)
		$Zeitspanne_von=$von;
	else
		$Zeitspanne_von=checkdatum('Zeitspanne_von');
	if(!$Zeitspanne_bis)
	{
		if($Zeitspanne_von && (substr($Zeitspanne_von,0,1) != '^'))
			$Zeitspanne_bis=ultimo($von,false).'240000';
		else
			$Zeitspanne_bis=$bis;
	}
	else
		$Zeitspanne_bis=checkdatum('Zeitspanne_bis');
	if((substr($Zeitspanne_von,0,1) != '^') && (substr($Zeitspanne_bis,0,1) != '^') && ($Zeitspanne_von > $Zeitspanne_bis))
	{
		$swap=$Zeitspanne_von;
		$Zeitspanne_von=$Zeitspanne_bis;
		$Zeitspanne_bis=$swap;
	}
}
$maskenfeldwerte['zeitspanne_von']=$Zeitspanne_von;
$maskenfeldwerte['nv_text1']='~-';
$maskenfeldwerte['zeitspanne_bis']=$Zeitspanne_bis;

// Person [Auswahl]
if(fxIsArray($_POST) && isset($_POST['Personen_ID']))
	$Personen_ID=(int)$_POST['Personen_ID'];
else
	$Personen_ID=$GLOBALS['fxpglobals']['person'];
$Pers_Arr=auswahl_Personen_ID(1);
$Personen_ID_ds=0;
if(sizeof($Pers_Arr) > 1)
	$Personen_ID_ds=1410;	// Alle Personen
$Personen_ID_arg=array('override'=>$Pers_Arr, 'Personen_ID'=>$GLOBALS['fxpglobals']['person']);
$maskenfeldwerte['personen_id']=$Personen_ID;

// Auftraggeber [Auswahl]
if(fxIsArray($_POST) && isset($_POST['Kunde']))
	$Kunde=(int)$_POST['Kunde'];
else
	$Kunde=0;
$Kunde_ds=1545;	// Alle Auftraggeber
$maskenfeldwerte['kunde']=$Kunde;

// Projekt [Auswahl]
if(fxIsArray($_POST) && isset($_POST['Projekt_ID']))
	$Projekt_ID=(int)$_POST['Projekt_ID'];
else
	$Projekt_ID=$GLOBALS['fxpglobals']['project'];
$Projekt_ID_arg=array('build'=>true, 'use_actual'=>true, 'start_date'=>$Zeitspanne_von, 'end_date'=>$Zeitspanne_bis, 'customers'=>$Kunde, 'persons'=>$Personen_ID, 'with_all'=>true, 'with_projects'=>true);
$Proj_Arr=auswahl_Projekt_ID(1);
$Projekt_ID_arg=array('override'=>$Proj_Arr);
$maskenfeldwerte['projekt_id']=$Projekt_ID;

// Max. Projektzeichen [Text]
$projekt_kuerzen='0';
if(fxIsArray($_POST) && isset($_POST['projekt_kuerzen']))
	$projekt_kuerzen=trim($_POST['projekt_kuerzen']);
$maskenfeldwerte['projekt_kuerzen']=$projekt_kuerzen;

// Max. Textzeichen [Text]
$text_kuerzen='50';
if(fxIsArray($_POST) && isset($_POST['text_kuerzen']))
	$text_kuerzen=trim($_POST['text_kuerzen']);
$maskenfeldwerte['text_kuerzen']=$text_kuerzen;

// [Checkboxes]
$cba=array('freigabe_ignorieren'=>0, 'Zeilenumbruch'=>0, 'Zeitdaten'=>156, 'Reisekosten'=>153, 'mit_SM'=>154, 'Anmerkung'=>0);
foreach($cba as $cbf => $cbt)
{
	if($GLOBALS['fxpglobals']['tr'] == $cbt)
	{
		$GLOBALS[$cbf]='1';
		$GLOBALS[$cbf.'_dis']=true;
	}
	else
	{
		$GLOBALS[$cbf]='';
		if(fxIsArray($_POST) && isset($_POST[$cbf]))
			$GLOBALS[$cbf]=trim($_POST[$cbf]);
		if(strlen($GLOBALS[$cbf]))
			$GLOBALS[$cbf]='1';
	}

	$maskenfeldwerte[strtolower($cbf)]=$GLOBALS[$cbf];
}
?>