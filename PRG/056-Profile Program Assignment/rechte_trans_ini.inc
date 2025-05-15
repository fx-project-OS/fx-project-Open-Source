<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : rechte_trans_ini.inc                                         //
// Version     : 24.1                                                         //
// Begin       : 2020-10-12                                                   //
// Last Change : 2020-10-13                                                   //
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
 * PF 56: Profile Program Assignment - Initialization
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$bit=fxf_getBitArray();
$sa=array();
$spa=array();
$arr=fxf_getClientData();

// Standard-Admin-Literal
$ladmin='fx-Admin';
if($GLOBALS['fxpglobals']['client'] == 1)
	$ladmin .= '-1';

// inactive
$inact=' (*'.meldung(299).')';	// (*inaktiv)
// standard
$stand=' (*fx-'.meldung(1700).')';	// (*fx-Standard)

// Handling of profile selection mask
$sarr=array(0=>'['.meldung(1422).']', 1=>$ladmin.$stand);
if(sizeof($arr['pga']))
{
	foreach($arr['pga'] as $gid => $ga)
	{
		$sarr[$gid]=$ga['nam'];
		if(!$ga['act'])
			$sarr[$gid] .= $inact;
	}
}
fxDebug($sarr,'$sarr', 3);

$Benutzergr_ID_ds=1422; // All
$Benutzergr_ID_arg=array('arr'=>$sarr);
if(fxIsArray($_POST) && fxIsArray($_POST['Benutzergr_ID']))
{
	$maskenfeldwerte['benutzergr_id']=$_POST['Benutzergr_ID'];
	if($_POST['Button_Select'] || $_POST['Button_Speichern'])
	{
		if(!$_POST['Benutzergr_ID'][0])
		{
			$spa=$sarr;
			unset($spa[0]);
		}
		else
		{
			foreach($sarr as $sid => $sname)
			{
				foreach($_POST['Benutzergr_ID'] as $pid)
				{
					if((int)$sid == (int)$pid)
						$spa[$sid]=$sname;
				}
			}
		}
	}
}
else
	$maskenfeldwerte['benutzergr_id']=array(0=>0);
fxDebug($spa,'$spa', 3);

if(!fxIsArray($_POST) || !sizeof($spa))
	$maske191_nv=true;
?>