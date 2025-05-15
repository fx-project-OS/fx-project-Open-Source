<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : personen_kompakt_pers_val.inc                                //
// Version     : 24.1                                                         //
// Begin       : 2020-10-07                                                   //
// Last Change : 2024-03-14                                                   //
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
 * PF 166: Client etc. - Person validation (will be called as an alternative program function from other person types as well)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$err_pers=false;

// Validate all fields
$vdf=false;
foreach($ppta as $ppf => $ppp)
{
	// Not hidden
	if(strpos($ppp,'#') === false)
	{
		// Field type: s=Select, t=Text, d=Date + c=Checkbox
		$ft=substr((string)$ppp,0,1);

		// Mandatory?
		$mand='0';
		if(strpos($ppp,'*') !== false)
			$mand='2';

		// Field "Valid from" or "Valid to"?
		if(($ppf == 'gueltig_von') || ($ppf == 'gueltig_bis'))
			$vdf=true;

		$verr=0;
		$_POST[$ppf]=validate_entry($_POST[$ppf], $verr, $fta[$ft], $mand);
		${$ppf}=$_POST[$ppf];
		if($verr)
		{
			error_msg($verr, -1, $ppf);
			$err_pers=true;
		}
		else if((($ppf == 'gueltig_von') || ($ppf == 'gueltig_bis')) && strlen((string)$_POST[$ppf]))
		{
			$_POST[$ppf]=substr((string)$_POST[$ppf],0,8);
			if($ppf == 'gueltig_von')
				$_POST[$ppf] .= '000000';
			else
				$_POST[$ppf] .= '240000';
			${$ppf}=$_POST[$ppf];
		}
	}
}

// Additional validation: Date "Valid from" cannot be greater then "Valid to"
if(!$err_pers && $vdf && strlen((string)$_POST['gueltig_von']) && strlen((string)$_POST['gueltig_bis']) && ($_POST['gueltig_von'] > $_POST['gueltig_bis']))
{
	// The end date [$var1] cannot be before the start date [$var2]!
	error_msg(1990, array('gueltig_von', 'gueltig_bis'), '<b>'.get_entry($_POST['gueltig_bis'],'date').'</b>', '<b>'.get_entry($_POST['gueltig_von'],'date').'</b>');
	$err_pers=true;
}

// Additional validation: Oneself cannot be their own superior
if(!$err_pers && ($GLOBALS['fxpglobals']['action'] != FXP_CREATE) && $_cid && $_pid && $_ppid && ($_pid == $_ppid))
{
	// A person cannot be assigned to oneself! Please choose another person or leave the field empty.
	error_msg(1919, -1, 'Elter');
	$err_pers=true;
}

// Special: Get new project and order customers if a customer's contact person has changed
if(!$err_pers && ($_ptype == FXP_PT_CUSTOMER_CP) && ($GLOBALS['fxpglobals']['action'] != FXP_CREATE) && $_cid && $_pid && $_ppid && ($_ppid != $_spda['prt']))
{
	$sql="SELECT projekt_id FROM projekte WHERE ansprechpartner=".$_pid." AND mandanten_id=".$_cid;
	$cust_project=(int)db_value($sql);
fxDebug($cust_project,'$cust_project: $sql='.$sql, 3);

	$sql="SELECT auftrags_id FROM auftrag WHERE ansprechpartner=".$_pid." AND mandanten_id=".$_cid;
	$cust_order=(int)db_value($sql);
fxDebug($cust_order,'$cust_order: $sql='.$sql, 3);
}

if($err_pers)
	$err=true;

if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) && $err)
	$_pid=0;
?>