<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : netzplan_ausw_ini.inc                                        //
// Version     : 21.2                                                         //
// Begin       : 2021-01-11                                                   //
// Last Change : 2021-01-11                                                   //
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
 * PF 226: Net Plan Diagram (NPD)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

if(isset($GLOBALS['fxptdata']['pdf']))
	unset($GLOBALS['fxptdata']['pdf']);

$err=false;
$verr=false;
$disp=true;

$sel=array();
$tva=array(
	'sMutterprojekt_ID'	=> "top_pid|0",
	'dSoll_Beg_Dtm'		=> "sdate|",		'ckritisch'			=> "cpath|1",		'sKunde'			=> "cust|0",
	'dSoll_End_Dtm'		=> "edate|",		'causr_arbeitstag'	=> "wd|1",			'sProjektmanager'	=> "pm|0",
	'ipuffertage'		=> "buffer|0",												'sProjektleiter'	=> "pl|0"
);

// Alle Variablen setzen
foreach($tva as $tfn => $tvn)
{
	$vp=strpos($tvn, '|');

	$tt=strtolower(substr($tfn, 0, 1));
	$tfn=substr($tfn, 1);

	$tdv=substr($tvn, $vp+1);
	$tvn=substr($tvn, 0, $vp);

	// ...Default value
	if(($tt == 's') || ($tt == 'i'))	// Select + Integer
		$tdv=(int)$tdv;
	else if($tt == 'f')					// Float
		$tdv=get_double($tdv);
	else								// Date, Checkbox + Text
	{
		$tdv=trim($tdv);
		if($tt == 'c')
		{
			if($tdv)
				$tdv='1';
			else
				$tdv='';
		}
	}

	if(isset($_POST) && is_array($_POST))
	{
		// Post
		if(isset($_POST[$tfn]))
		{
			if($tt == 's')		// Select
				${$tvn}=(int)$_POST[$tfn];
			else if($tt == 'c')	// Checkbox
				${$tvn}='1';
			else if($tt == 'd')	// Date
			{
				${$tvn}=validate_entry(trim($_POST[$tfn]), $verr, "datum");
				if($verr)
				{
					$err=true;
					error_msg($verr, false, $tfn);
				}
			}
			else if($tt == 'i')	// Integer
			{
				${$tvn}=trim($_POST[$tfn]);
				if(strlen(${$tvn}))
				{
					${$tvn}=validate_entry(trim($_POST[$tfn]), $verr, "ganzzahl", '1');
					if($verr)
					{
						$err=true;
						error_msg($verr, false, $tfn);
					}
					else
						${$tvn}=(int)${$tvn};
				}
			}
		}
		else if($tt == 'c')
			${$tvn}='';
	}

	// Default
	if(!isset(${$tvn}))
		${$tvn}=$tdv;
fxDebug(${$tvn},'$'.$tvn.' (Default: '.$tdv.')', 2);
}

// Netzplan nicht anzeigen, falls Fehler aufgetreten
if($err || !$top_pid || !fxIsArray($_POST) || !isset($_POST['Button_Anzeigen']))
	$disp=false;
?>