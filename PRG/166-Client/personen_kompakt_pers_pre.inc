<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : personen_kompakt_pers_pre.inc                                //
// Version     : 21.2                                                         //
// Begin       : 2020-10-07                                                   //
// Last Change : 2020-10-07                                                   //
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
 * PF 166: Client etc. - Person (will be called as an alternative program function from other person types as well)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$GLOBALS['fxptdata']['ignore_date_boundaries']=true;

if(isset($index))
	unset($index);
if(isset($maskenfeldwerte))
	unset($maskenfeldwerte);

if(!$err && ($GLOBALS['fxpglobals']['action'] == FXP_DELETE) && $_POST['Button_PGLoeschen'])
	$_pid=0;
if(!$_pid && ($GLOBALS['fxpglobals']['action'] != FXP_CREATE))
	$maske383_nv=true;

// Set line height
if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
{
	$maske383_dso=true;
	$maske383_trheight=25;
	$maske383_hr_height=6;
}

// Hide and/or disable fields and/or define mandatory fields
foreach($ppta as $ppf => $ppp)
{
	// Hide?
	if(strpos($ppp,'#') !== false)
		${$ppf.'_nv'}=true;
	// Disabled?
	if(strpos($ppp,'!') !== false)
		${$ppf.'_dis'}=true;
	// Mandatory?
	if(strpos($ppp,'*') !== false)
		${$ppf.'_muss'}=true;
}

// Restore values on error
if($err)
{
	foreach($ppta as $ppf => $ppp)
	{
		// Not hidden?
		if(strpos($ppp,'#') === false)
			$maskenfeldwerte[strtolower($ppf)]=$_POST[$ppf];
	}
}
else if($_cid)
{
	if($_pid)
	{
		$sql="SELECT * FROM personen WHERE personen_id=".$_pid." AND mandanten_id=".$_cid;
		$maskenfeldwerte=db_values($sql);
		if($maskenfeldwerte['inaktiv'])
			$maskenfeldwerte['inaktiv']='1';
		else
			$maskenfeldwerte['inaktiv']='';
	}
	else
	{
		$sql="SELECT abt_id FROM personen WHERE personen_id=".$GLOBALS['fxpglobals']['person']." AND mandanten_id=".$_cid;
		$maskenfeldwerte['abt_id']=(int)db_value($sql);
		$maskenfeldwerte['elter']=$GLOBALS['fxpglobals']['person'];
	}
}

// Salutation [Anrede]
if(!isset($Anrede_nv) || !$Anrede_nv)
{
	if(isset($Anrede_muss) && $Anrede_muss)
	 	$Anrede_ds=0;
	if(!$err && ((fxIsArray($_GET) && isset($_GET['uid'])) || !fxIsArray($maskenfeldwerte) || !isset($maskenfeldwerte['anrede'])))
	{
		if(sizeof($pppa['persontypes']))
			$maskenfeldwerte['anrede']=2803;	// Mr./Ms.
		else
			$maskenfeldwerte['anrede']=310;		// Company
	}
}

// Sex [Geschlecht]
if(!isset($Geschlecht_nv) || !$Geschlecht_nv)
{
	if(isset($Geschlecht_muss) && $Geschlecht_muss)
		$Geschlecht_ds=0;
	if(!$err && ((fxIsArray($_GET) && isset($_GET['uid'])) || !fxIsArray($maskenfeldwerte) || !isset($maskenfeldwerte['geschlecht'])))
		$maskenfeldwerte['geschlecht']=FXP_GT_DIVERSE;	// Diverse
}

// Change to Customer [Kunde] (... Only contacts can be changed to customers)
if(($_ptype == FXP_PT_CONTACT) && ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
{
	if(isset($_POST) && $_POST['Kunde'])
		$maskenfeldwerte['kunde']='1';
	else
		$maskenfeldwerte['kunde']='';
}
else
{
	$Kunde_lit='&nbsp;';
	$Kunde_nc=true;
	$maskenfeldwerte['kunde']='~';
}

// Allocated to [Elter]
if(!isset($Elter_nv) || !$Elter_nv)
{
	if(!isset($_ppid))
		$_ppid=0;
	if(!$_ppid && $_cid)
	{
		if($_pid)
			$_ppid=(int)db_value("SELECT elter FROM personen WHERE personen_id=".$_pid." AND mandanten_id=".$_cid);
		else if($_ptype == FXP_PT_CLIENT_CP)
			$_ppid=(int)db_value("SELECT personen_id FROM personen WHERE personentyp=".FXP_PT_CLIENT." AND mandanten_id=".$_cid);
	}
	$maskenfeldwerte['elter']=$_cid.'_'.$_ppid;
	$Elter_arg=array('modus'=>2, 'ptpa'=>$pppa);
	if($_ppid > 0)
		$Elter_arg['pid']=$maskenfeldwerte['elter'];
	if(isset($Elter_muss) && $Elter_muss)
	 	$Elter_ds=0;
fxDebug($Elter_arg,'$Elter_arg: $maskenfeldwerte[\'elter\']='.$maskenfeldwerte['elter'], 3);
}

// ...Picture [bild]
$_name=convertFilename(trim($maskenfeldwerte['pname']).trim($maskenfeldwerte['vorname']));
$_sex=(int)$maskenfeldwerte['geschlecht'];
echo(fieldHidden('uparam','PER|'.$_cid.'|'.$GLOBALS['fxpglobals']['action'].'|'.$_pid.'|'.$_sex.'|'.$_name));
$bild_htext=get_text(1567,'10').' ['.meldung($_ptype).']';
if($err)
	$maskenfeldwerte['bild']=$_POST['bild'];
else
	$maskenfeldwerte['bild']=getPersonIcons($_pid, 80, 0, $_cid, $_ptype, true);
fxDebug($maskenfeldwerte,'$maskenfeldwerte', 3);

// Display parameter for cost accounting
$kst_art=908;
?>