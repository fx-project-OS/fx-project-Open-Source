<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : sprojects.inc                                                //
// Version     : 21.2                                                         //
// Begin       : 2020-11-06                                                   //
// Last Change : 2020-11-06                                                   //
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
 * Adjust selected and selectable projects according to submitted values
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

// Search for and include "basics.inc" to set all definitions, variables and necessary dynamic paths
$__pnm='basics.inc';
if(!isset($GLOBALS['__loaded_'.$__pnm]))
{
	$__prg=$__pnm;
	$__pfc=0;
	while(($__pfc < 10) && !file_exists($__prg))
	{
		$__pfc++;
		$__prg='../'.$__prg;
	}
	if(file_exists($__prg))
	{
		require($__prg);
	}
	else
		die('<div style="padding:8px;"><h1 style="padding:8px;color:red;background:#fff;border:2px solid red;box-shadow:4px 4px 4px rgba(0,0,0, 0.5);">fx-project - ERROR: Necessary file &quot;'.$__pnm.'&quot; not found!</h1></div>');
}

// Was program call by JavaScript/AJAX or included
$callAJAX=false;
if(!isset($s_name))
	$s_name='';
if(isset($_GET) && isset($_GET['s_name']))
{
	$s_name=$_GET['s_name'];
	$callAJAX=true;
}
if(!isset($s_val))
	$s_val=0;
if(isset($_GET) && isset($_GET['s_val']))
{
	$s_val=(int)$_GET['s_val'];
	$callAJAX=true;
}

// Function variables
$va=array(
	'imulti'=>0,

	'iuse_actual'=>0,
	'dstart_date'=>'', 'dend_date'=>'', 'tcustomers'=>'', 'tpersons'=>'',
	'iwith_all'=>0, 'iwith_projects'=>0
);
foreach($va as $vn => $vd)
{
	$vt=substr($vn,0,1);
	$vn=substr($vn,1);
	if(!isset($GLOBALS[$vn]))
	{
		if(isset($_GET) && isset($_GET[$vn]))
		{
			if($vt == 'i')
				$GLOBALS[$vn]=(int)$_GET[$vn];
			else
				$GLOBALS[$vn]=$_GET[$vn];
		}
		else
			$GLOBALS[$vn]=$vd;
	}
}

// Select field variables
$Projekt_ID_arg=array(
	'build'=>true,
	'multi'=>$GLOBALS['multi'],
	'use_actual'=>$GLOBALS['use_actual'],
	'start_date'=>$GLOBALS['start_date'], 'end_date'=>$GLOBALS['end_date'],
	'customers'=>$GLOBALS['customers'],
	'persons'=>$GLOBALS['persons'],
	'with_all'=>$GLOBALS['with_all'],
	'with_projects'=>$GLOBALS['with_projects']
);
//fxDebug($Projekt_ID_arg,'$Projekt_ID_arg', 0);
$rarr=auswahl_Projekt_ID(1);
if(!fxIsArray($rarr))
{
	if($GLOBALS['with_all'])
		$rarr[0]=meldung(1077,false);	// All Projects
	else
		$rarr[0]=meldung(794,true);		// -empty-
}
//fxDebug($rarr,'$rarr', 0);

// AJAX: Save submitted values and return selection field
if($callAJAX)
{
	// Load selects
	fxSession('s','l');
	// ...create new select
	$n_val='0';
	$GLOBALS['fxpselects'][$s_name]=array();
	if(fxIsArray($rarr))
	{
		foreach($rarr as $value => $text)
		{
			$GLOBALS['fxpselects'][$s_name][$value]=$text;
			if($value == $s_val)
				$n_val=(string)$value;
		}
	}
	// ...and save it
	fxSession('s','s');

	// Load transaction values
	$frm='';
	if(is_array($_GET) && isset($_GET['frm']))
		$frm=trim($_GET['frm']);
	fxSession('t','l','ret');
	$GLOBALS['_form']=$frm;
	// ...set new values
	fxSession('t','p',$s_name,$n_val);							// Project
	fxSession('t','p','Zeitspanne_von',$GLOBALS['start_date']);	// Start date
	fxSession('t','p','Zeitspanne_bis',$GLOBALS['end_date']);	// End date
	fxSession('t','p','Kunde',$GLOBALS['customers']);			// Customers
	fxSession('t','p','Personen_ID',$GLOBALS['persons']);		// Persons
	// ...and save it
	fxSession('t','s');

	if($GLOBALS['multi'])
		$result=multiSelEntries($s_name,$rarr);
	else
		$result=selEntriesJS($s_name,$rarr,false,false,true);
//echo('<pre>'.htmlEncode($result).'</pre>');
	echo($result);
}
// INCLUDE: Set selection array
else
{
	$GLOBALS[$s_name.'_array']=$rarr;
}
?>