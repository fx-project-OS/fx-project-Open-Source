<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : personen_kompakt_man_pre.inc                                 //
// Version     : 21.1                                                         //
// Begin       : 2020-10-05                                                   //
// Last Change : 2020-10-05                                                   //
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
 * PF 166: Client definition
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

setForm(true);

if(isset($index))
	unset($index);
if(isset($maskenfeldwerte))
	unset($maskenfeldwerte);

$client_arr=array();

// Mandatory fields
if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
{
	$Pname_muss=true;
	$Name_Benutzer_muss=true;
}

// Hide fields
if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
{
	$Passwort_alias_nv=true;
	$Passwort_check_nv=true;
}
if($GLOBALS['fxpglobals']['action'] != FXP_CREATE)
{
	$Mandanten_ID_Neu_nv=true;
	$quelle_nv=true;
	$Nachschlage_Art_nv=true;
}
if(($GLOBALS['fxpglobals']['action'] == FXP_CHANGE) && ($_cid == $GLOBALS['fxpglobals']['client']))
{
	$gueltig_von_nv=true;
	$gueltig_bis_nv=true;
	$inaktiv_nv=true;
}
if($quelle_nv && $Nachschlage_Art_nv && $gueltig_von_nv && $gueltig_bis_nv && $inaktiv_nv)
{
	$hr0_nv=true;
	unset($quelle_nv);
	$quelle_lit='&nbsp;';
	$quelle_nc=true;
}

// Set line height
if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
{
	$maske394_dso=true;
	$maske394_trheight=24;
	$maske394_hr_height=6;
}

// Restore submitted values on error
if($err)
{
	$feldarray=array('Pname', 'Name_Zusatz', 'Personen_Initialen', 'Nachschlage_Art', 'gueltig_von', 'gueltig_bis', 'inaktiv', 'Name_Benutzer', 'Passwort_alias', 'E_Mail_Adresse', 'Passwort_check');
	foreach($feldarray as $faname)
	{
		$lfaname=strtolower($faname);
		$maskenfeldwerte[$lfaname]=$_POST[$faname];
	}
//fxDebug($maskenfeldwerte, '$maskenfeldwerte', 0);

	if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
	{
		$maskenfeldwerte['pname']='~<b>'.$_POST['Pname'].'</b>'.fieldHidden('Pname',$_POST['Pname']);
		if(strlen($_POST['gueltig_von']))
			$maskenfeldwerte['gueltig_von']='~'.$_POST['gueltig_von'];
		if(strlen($_POST['gueltig_bis']))
			$maskenfeldwerte['gueltig_bis']='~'.$_POST['gueltig_bis'];
	}
}

// Create
if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
{
	// Preset next free client number
	if(!strlen($_POST['Mandanten_ID']) || !$err)
	{
		$sql="SELECT MAX(mandanten_id) FROM personen WHERE personentyp=".FXP_PT_CLIENT." AND mandanten_id>0";
		$_ncid=(int)db_value($sql) + 1;

		$maskenfeldwerte['name_benutzer']='admin';
	}
	else
		$ncid=$_POST['Mandanten_ID'];

	$maskenfeldwerte['mandanten_id']=$_ncid;
}

// Get values from database
else if($_pid)
{
	// Client Id
	$maskenfeldwerte['mandanten_id']=$_cid;

	if(!$err || ($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
	{
		// Client data
		$sql="SELECT pname, name_zusatz, personen_initialen, inaktiv, gueltig_von, gueltig_bis FROM personen WHERE mandanten_id=".$_cid." AND personentyp=".FXP_PT_CLIENT;
		$client_arr=db_values($sql);
fxDebug($client_arr,$sql, 3);
		if(fxIsArray($client_arr))
		{
			if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
				$maskenfeldwerte['pname']='~<b>'.$client_arr['pname'].'</b>'.fieldHidden('Pname',$client_arr['pname']);
			else
				$maskenfeldwerte['pname']=$client_arr['pname'];
			$maskenfeldwerte['name_zusatz']=$client_arr['name_zusatz'];
			$maskenfeldwerte['personen_initialen']=$client_arr['personen_initialen'];
			$maskenfeldwerte['gueltig_von']=$client_arr['gueltig_von'];
			$maskenfeldwerte['gueltig_bis']=$client_arr['gueltig_bis'];
			if($client_arr['inaktiv'])
				$maskenfeldwerte['inaktiv']=1;
			else
				$maskenfeldwerte['inaktiv']='';
		}
		else
			$client_arr=array();

		// Admin daten
		$maskenfeldwerte['passwort_alias']='';
		$maskenfeldwerte['passwort_check']='';
		$Admin_sql="SELECT name_benutzer, e_mail_adresse FROM benutzer WHERE mandanten_id=".$_cid." AND benutzer_id=1";
		$Admin_arr=db_values($Admin_sql);
		if(fxIsArray($Admin_arr))
		{
			$maskenfeldwerte['name_benutzer']=$Admin_arr['name_benutzer'];
			$maskenfeldwerte['e_mail_adresse']=$Admin_arr['e_mail_adresse'];
		}
	}
}

// ...Picture [bild]
$pf_pid=0;
$pf_name='';
$pf_gnd=0;
if($_pid)
	$pf_pid=(int)$_pid;
if(fxIsArray($Person_arr))
{
	$pf_name=convertFilename(trim($Person_arr['pname']).trim($Person_arr['vorname']));
	$pf_gnd=(int)$Person_arr['geschlecht'];
}
else if(fxIsArray($_POST) && isset($_POST['Pname']))
{
	$pf_name=convertFilename(trim($_POST['Pname']).trim($_POST['Vorname']));
	$pf_gnd=(int)$_POST['Geschlecht'];
}
else if($pf_pid)
{
	$sql="SELECT pname, vorname, geschlecht FROM personen WHERE personen_id=".$pf_pid." AND mandanten_id=".$_cid;
	$arr=db_values($sql);
	if(fxIsArray($arr))
	{
		$pf_name=convertFilename(trim($arr['pname']).trim($arr['vorname']));
		$pf_gnd=(int)$arr['geschlecht'];
	}
}
echo(fieldHidden('uparam','PER|'.$_cid.'|'.$GLOBALS['fxpglobals']['action'].'|'.$pf_pid.'|'.$pf_gnd.'|'.$pf_name));
$bild_htext=get_text(1567,'10').' ['.meldung(FXP_PT_CLIENT,false).']';
if($err)
	$maskenfeldwerte['bild']=$_POST['bild'];
else
	$maskenfeldwerte['bild']=getPersonIcons($pf_pid, 80, 0, $_cid, FXP_PT_CLIENT, true);
?>