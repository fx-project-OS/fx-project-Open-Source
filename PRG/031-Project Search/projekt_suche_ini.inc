<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projekt_suche_ini.inc                                        //
// Version     : 21.1                                                         //
// Begin       : 2020-12-22                                                   //
// Last Change : 2020-12-22                                                   //
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
 * PF 31: Project Search
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

if($_POST['Button_suchen'])
{
	$err_datum=false;

	// Suchtext
	$st=trim($_POST['Suchtext']);

	// Suche von
	$sv=trim($_POST['suche_von']);
	if(strlen($sv))
	{
		$sv=validate_entry($sv, $verr, 'datum');
		if($verr)
		{
			error_msg($verr, false, 'suche_von');
			$err=true;
			$err_datum=true;
		}
		else
			$sv=substr($sv, 0, 8).'000000';
	}

	// Suche bis
	$sb=trim($_POST['suche_bis']);
	if(strlen($sb))
	{
		$sb=validate_entry($sb, $verr, 'datum');
		if($verr)
		{
			error_msg($verr, false, 'suche_bis');
			$err=true;
			$err_datum=true;
		}
		else
			$sb=substr($sb, 0, 8).'240000';
	}

	// Art des Vorgangs
	$pa=(int)$_POST['Projektart'];

	// Projektleiter
	$pl=(int)$_POST['Projektleiter'];

	// Auftraggeber
	$ag=(int)$_POST['Kunde'];

	// Name des Vorgangs
	$cn=trim($_POST['Name_Projekt']);

	// PCA-Code
	$cp=trim($_POST['pca_code']);

	// ID
	$ci=trim($_POST['ID']);

	// Checkbox
	if(strlen($st) && !strlen($cn) && !strlen($cp) && !strlen($ci))
	{
		// 1646: Es wurde keine notwendige Checkbox markiert!
		error_msg(1646, array('Name_Projekt', 'pca_code', 'ID'));
		$err=true;
	}

	// Logikprüfung: Datumsbeziehung
	if(!$err_datum && strlen($sv) && strlen($sb) && ($sb < $sv))
	{
		// 551: Das Enddatum liegt vor dem Startdatum!
		error_msg(551, array('suche_von', 'suche_bis'));
		$err=true;
	}
}
else
{
	$err=true;

	if(fxIsArray($GLOBALS['fxptdata']['backup']))
		extract($GLOBALS['fxptdata']['backup']);
	else
	{
		$st='';
		$sv='';
		$sb='';
		$pa=0;
		$pl=0;
		$ag=0;
		$cn='1';
		$cp='';
		$ci='';
	}
}

$GLOBALS['fxptdata']['backup']=array
(
	'st'=>$st,
	'sv'=>$sv,
	'sb'=>$sb,
	'pa'=>$pa,
	'pl'=>$pl,
	'ag'=>$ag,
	'cn'=>$cn,
	'cp'=>$cp,
	'ci'=>$ci
);
//fxDebug($GLOBALS['fxptdata']['backup'], $tr);
?>