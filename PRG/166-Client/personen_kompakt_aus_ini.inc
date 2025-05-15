<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : personen_kompakt_aus_ini.inc                                 //
// Version     : 24.1                                                         //
// Begin       : 2020-10-02                                                   //
// Last Change : 2024-07-08                                                   //
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
 * PF 166: Client etc. - Additional initialization file (will be called as an alternative program function from other person types as well)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

//fxDebug('_GET'); fxDebug('_POST');

// Default variables
$cust_project=0;
$cust_order=0;
$set_header_width=true;
$inaktiv_alias=$GLOBALS['fxpglobals']['filter']['inaktiv'];
$dbz_arr=array();

// Images
$cb_new='<span class="s4 bold darkgrey">+</span>';
$cb_lst='<span class="s3 bold lightergrey">&rarr;</span>';

// Client and person Id's
$_cid=0;
$_pid=0;
$_btr=0;
$_bcid=0;
$_bpid=0;
// ...Extract backed up values
if(fxIsArray($GLOBALS['fxptdata']['backup']))
{
	if(fxIsArray($GLOBALS['fxptdata']['backup']['_POST']))
		unset($GLOBALS['fxptdata']['backup']['_POST']);
	extract($GLOBALS['fxptdata']['backup']);
}
// ...Call from different program function
if(isset($_GET) && $_GET['pid'] && ($_GET['ptyp'] || (fxIsArray($GLOBALS['fxpglobals']['trstack']) && ($_GET['ltr'] == $GLOBALS['fxpglobals']['trstack'][1]))))
{
	$_cid=(int)$GLOBALS['fxpglobals']['client'];
	$_pid=(int)$_GET['pid'];

	$_POST['Person_waehlen']=true;
}
// ...Values submitted
else if(($_btr == $GLOBALS['fxpglobals']['tr']) && fxIsArray($_POST) && isset($_POST['Person_alias']))
{
	$pe=explode('_', $_POST['Person_alias']);
	if(sizeof($pe) == 2)
	{
		$_cid=(int)$pe[0];
		$_pid=(int)$pe[1];
	}
	else
	{
		$_cid=(int)$GLOBALS['fxpglobals']['client'];
		$_pid=(int)$_POST['Person_alias'];
	}
}


// Superior's person Id (= parent element)
$_pcid=0;
$_ppid=0;
if(fxIsArray($_POST) && isset($_POST['Elter']))
{
	$pe=explode('_', $_POST['Elter']);
	if(sizeof($pe) == 2)
	{
		$_pcid=(int)$pe[0];
		$_ppid=(int)$pe[1];
	}
	else
	{
		$_pcid=$_cid;
		$_ppid=(int)$_POST['Elter'];
	}
}

// No client/person Id?
if(!$_cid)
{
	if($_pcid && ($_ptype == FXP_PT_CLIENT_CP))
		$_cid=$_pcid;
	else
		$_cid=$GLOBALS['fxpglobals']['client'];
	$_pid=0;
}
else if(($_pid <= 0) || ($GLOBALS['fxpglobals']['action'] == FXP_CREATE))
	$_pid=0;
fxDebug(array('$_cid'=>$_cid, '$_pid'=>$_pid),'Client and Person Id', 2);

// Check if user is allowed to display and/or change the selected person [except for program function 164 and 166 = client data]
if(($GLOBALS['fxpglobals']['action'] > FXP_CREATE) && $_pid && ($GLOBALS['fxpglobals']['tr'] != 164) && ($GLOBALS['fxpglobals']['tr'] != 166))
{
	$pr=false;
	$cpa=explode(',', get_persons());
	foreach($cpa as $pid)
	{
		if($pid == $_pid)
		{
			$pr=true;
			break;
		}
	}

	if(!$pr)
	{
		$_pid=0;
		$_POST['Person_waehlen']=true;
	}
}

// Display person immediately
if($_cid && $_pid && ($GLOBALS['fxpglobals']['action'] != FXP_CREATE) && (!fxIsArray($_POST) || (!isset($_POST['Button_Multi']) && !isset($_POST['Button_PGLoeschen']) && !isset($_POST['Button_PZLoeschen']))))
	$_POST['Person_waehlen']=true;

// Person selected
if($_POST['Person_waehlen'])
	unset($db_update);
// Check if person selection has changed
else if($db_update && ($GLOBALS['fxpglobals']['action'] >= FXP_CHANGE) && (($_cid != $_bcid) || ($_pid != $_bpid)))
{
	error_msg(2348);	// The selection criteria have been changed. The data record has been reloaded.
	$_POST['Person_waehlen']=true;
	unset($db_update);
}

// Get certain values for selected person
$_spda=array('typ'=>0, 'gnd'=>0, 'dpt'=>0, 'prt'=>0, 'nam'=>"", 'fnm'=>"", 'tnm'=>"", 'uid'=>0, 'unm'=>"", 'sub'=>"", 'adr'=>0, 'com'=>0);
if($_cid && $_pid)
{
	$sql="SELECT personentyp, geschlecht, abt_id, elter, pname, vorname FROM personen WHERE personen_id=".$_pid." AND mandanten_id=".$_cid;
	$tmp=db_values($sql);
fxDebug($tmp,$sql, 3);
	if(fxIsArray($tmp))
	{
		$_spda['typ']=(int)$tmp['personentyp'];
		$_spda['gnd']=(int)$tmp['geschlecht'];
		$_spda['dpt']=(int)$tmp['abt_id'];
		$_spda['prt']=(int)$tmp['elter'];
		$_spda['nam']=$tmp['pname'];
		$_spda['fnm']=$tmp['vorname'];
		if($_spda['fnm'] === NULL)
			$_spda['fnm']='';

		$_spda['tnm']=$tmp['pname'];
		if(strlen((string)$_spda['fnm']))
			$_spda['tnm'] .= ', '.$_spda['fnm'];
	}


	$sql="SELECT benutzer_id, name_benutzer FROM benutzer WHERE personen_id=".$_pid." AND mandanten_id=".$_cid;
	$tmp=db_values($sql);
fxDebug($tmp,$sql, 3);
	if(fxIsArray($tmp))
	{
		$_spda['uid']=(int)$tmp['benutzer_id'];
		$_spda['unm']=$tmp['name_benutzer'];
	}

	$sql="SELECT pname, vorname FROM personen WHERE elter=".$_pid." AND mandanten_id=".$_cid." ORDER BY pname, vorname";
	$tmp=db_values($sql, '*');
fxDebug($tmp,$sql, 3);
	if(fxIsArray($tmp))
	{
		$_spda['sub']='<ul>';
		foreach($tmp as $t)
		{
			$_spda['sub'] .= '<li><b>'.$t['pname'].'</b>';
			if(($t['vorname'] !== NULL) && strlen((string)$t['vorname']))
				$_spda['sub'] .= ', '.$t['vorname'];
			$_spda['sub'] .= '</li>';
		}
		$_spda['sub'] .= '</ul>';
	}

	$sql="SELECT COUNT(adressen_id) AS adr FROM adressen WHERE personen_id=".$_pid." AND mandanten_id=".$_cid;
	$_spda['adr']=(int)db_value($sql);
fxDebug($_spda['adr'],$sql, 3);

	$sql="SELECT COUNT(kommunikation_id) AS com FROM kommunikation WHERE personen_id=".$_pid." AND mandanten_id=".$_cid;
	$_spda['com']=(int)db_value($sql);
fxDebug($_spda['com'],$sql, 3);
}
fxDebug($_spda,'$_spda', 2);

// Count Client's Contact Persons (= Administrators)
$ccc=0;
if(($_ptype == FXP_PT_CLIENT_CP) && $_cid)
	$ccc=(int)db_value("SELECT COUNT(personen_id) FROM personen WHERE personentyp=".FXP_PT_CLIENT_CP." AND mandanten_id=".$_cid);

// Backup variables
$GLOBALS['fxptdata']['backup']['_btr']=$GLOBALS['fxpglobals']['tr'];
$GLOBALS['fxptdata']['backup']['_bcid']=$_cid;
$GLOBALS['fxptdata']['backup']['_bpid']=$_pid;

// Superior's person Id (= parent element)
if(!$_ppid && $_spda['prt'])
	$_ppid=$_spda['prt'];
fxDebug(array('$_ppid'=>$_ppid),'Superior person Id', 2);

// Field types array
$fta=array('s'=>"select", 't'=>"text", 'd'=>"date", 'c'=>"checkbox");

// Person parameters depending on person type
// ...field types: s=Select, t=Text, d=Date + c=Checkbox
// ...parameters: *=Mandatory, #=NotVisible + !=Disabled
$ppta=array(
	'Anrede'=>'s',
	'Geschlecht'=>'s',

	'Pname'=>'t',
	'Vorname'=>'t',
	'Name_Zusatz'=>'t',
	'Personen_Initialen'=>'t',

	'Abt_ID'=>'s',
	'Pposition'=>'s',
	'PersTitel'=>'s',

	'Elter'=>'s',

	'gueltig_von'=>'d',
	'gueltig_bis'=>'d',
	'inaktiv'=>'c'
);
// ...Salutation [Anrede]
// ...Sex [Geschlecht]
if(($_ptype == FXP_PT_CLIENT_CP) || ($_ptype == FXP_PT_CUSTOMER_CP) || ($_ptype == FXP_PT_PARTNER_CP) || ($_ptype == FXP_PT_EMPLOYEE) || ($_ptype == FXP_PT_CONTRACTOR) || ($_ptype == FXP_PT_CONTACT_CP))
{
	$ppta['Anrede'] .= '*';
	$ppta['Geschlecht'] .= '*';
}
// ...Name [Pname]
$ppta['Pname'] .= '*';
// ...First Name [Vorname]
//if($_ptype == FXP_PT_CLIENT_CP)
//	$ppta['Vorname'] .= '*';
// ...Name affix [Name_Zusatz]
//if($_ptype == FXP_PT_CLIENT_CP)
//	$ppta['Name_Zusatz'] .= '*';
// ...Identif. No. [Personen_Initialen]
//if($_ptype == FXP_PT_CLIENT_CP)
//	$ppta['Personen_Initialen'] .= '*';
// ...Department [Abt_ID]
// ...Job Description [Pposition]
if(($_ptype == FXP_PT_CLIENT_CP) || ($_ptype == FXP_PT_CUSTOMER) || ($_ptype == FXP_PT_CUSTOMER_CP) || ($_ptype == FXP_PT_PARTNER) || ($_ptype == FXP_PT_PARTNER_CP) || ($_ptype == FXP_PT_CONTACT) || ($_ptype == FXP_PT_CONTACT_CP))
{
	$ppta['Abt_ID'] .= '#';
	$ppta['Pposition'] .= '#';
}
// ...Title [PersTitel]
//if($_ptype == FXP_PT_CLIENT_CP)
//	$ppta['PersTitel'] .= '*';
// ...Allocated to [Elter]
//if(($_ptype == FXP_PT_CLIENT_CP) || ($_ptype == FXP_PT_CUSTOMER) || ($_ptype == FXP_PT_CUSTOMER_CP) || ($_ptype == FXP_PT_CONTACT) || ($_ptype == FXP_PT_CONTACT_CP))
	$ppta['Elter'] .= '*';
if((($_ptype == FXP_PT_CLIENT_CP) && ($GLOBALS['fxpglobals']['action'] != FXP_CREATE)) || ($_ptype == FXP_PT_CUSTOMER) || ($_ptype == FXP_PT_PARTNER))
	$ppta['Elter'] .= '!';
// ...Valid from [gueltig_von]
//if($_ptype == FXP_PT_CLIENT_CP)
//	$ppta['gueltig_von'] .= '*';
// ...Valid to [gueltig_bis]
//if($_ptype == FXP_PT_CLIENT_CP)
//	$ppta['gueltig_bis'] .= '*';
// ...Inactive [inaktiv]
if((($_ptype == FXP_PT_CLIENT_CP) && ($_cid < 2)) || (($_cid == $GLOBALS['fxpglobals']['client']) && ($_pid == $GLOBALS['fxpglobals']['person'])))
	$ppta['inaktiv'] .= '!';

// Parent persontypes
$pppa=array(
	'persontypes'=>array(),
	'all_clients'=>$all_clients
);
if(($_ptype == FXP_PT_CLIENT_CP) || ($_ptype == FXP_PT_CUSTOMER) || ($_ptype == FXP_PT_PARTNER) || ($_ptype == FXP_PT_CONTACT) || ($_ptype == FXP_PT_EMPLOYEE) || ($_ptype == FXP_PT_CONTRACTOR))
	$pppa['persontypes'][]=FXP_PT_CLIENT;
if(($_ptype == FXP_PT_CUSTOMER_CP) || ($_ptype == FXP_PT_CONTRACTOR))
	$pppa['persontypes'][]=FXP_PT_CUSTOMER;
else if($_ptype == FXP_PT_PARTNER_CP)
	$pppa['persontypes'][]=FXP_PT_PARTNER;
else if($_ptype == FXP_PT_CONTACT)
	$pppa['persontypes'][]=FXP_PT_EMPLOYEE;
else if($_ptype == FXP_PT_CONTACT_CP)
	$pppa['persontypes'][]=FXP_PT_CONTACT;
else if($_ptype == FXP_PT_EMPLOYEE)
	$pppa['persontypes'][]=FXP_PT_EMPLOYEE;
else if($_ptype == FXP_PT_CONTRACTOR)
	$pppa['persontypes'][]=FXP_PT_PARTNER;


// Buttons
$Button_PZLoeschen_btxt='';
$Button_PGLoeschen_btxt='';
if(($GLOBALS['fxpglobals']['action'] != FXP_CREATE) && ($GLOBALS['fxpglobals']['action'] != FXP_CHANGE))
	$Button_Multi_nv=true;
if($GLOBALS['fxpglobals']['action'] != FXP_DELETE)
{
	$Button_PZLoeschen_nv=true;
	$Button_PGLoeschen_nv=true;
}
else
{
	// Delete Address and Communication Data
	if($_spda['adr'] || $_spda['com'])
		$Button_PZLoeschen_btxt=get_text(1852,'text');
	else
		$Button_PZLoeschen_nv=true;
	if(!$_spda['adr'])
		$maske384_nv=true;
	if(!$_spda['com'])
		$maske386_nv=true;

	// Deletion of client's (only) admin is prohibited
	if((($_ptype == FXP_PT_CLIENT_CP) && ($ccc < 2)) || ($_spda['uid'] == 1))
		$Button_PGLoeschen_nv=true;
	// Deletion of client allowed?
	else if(($_ptype == FXP_PT_CLIENT) && (($_cid <= 1) || ($GLOBALS['fxpglobals']['client'] > 1)))
		$Button_PGLoeschen_nv=true;
	else
	{
		$pgt='20';				// Delete Person
		if(($_spda['gnd'] != FXP_GT_MALE) && ($_spda['gnd'] != FXP_GT_FEMALE) && ($_spda['gnd'] != FXP_GT_DIVERSE))
		{
			if(($_spda['typ'] == FXP_PT_CLIENT) || ($_spda['typ'] == FXP_PT_CUSTOMER) || ($_spda['typ'] == FXP_PT_PARTNER) || ($_spda['typ'] == FXP_PT_CONTACT))
				$pgt='15';		// Delete Company
			else if(!$_spda['typ'])
				$pgt='text';	// Delete Person/Company incl. Additional Data
		}
		$Button_PGLoeschen_btxt=get_text(1851,$pgt);
	}
}

// Messages
// ...Client
if(($_ptype == FXP_PT_CLIENT) && ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
{
	// It is not possible to delete the whole client $var1! Only additional data can be deleted.
	if((($_cid <= 1) || ($GLOBALS['fxpglobals']['client'] > 1)))
	{
		if(fxIsArray($_POST) && isset($_POST['Person_waehlen']))
			error_msg(111, false, $_cid);
	}
	// Warning! With this action all entries in all tables of this client will be irreversible deleted! Please click again on [$var1] if you are absolutely sure and want to delete this data.
	else if(fxIsArray($_POST) && isset($_POST['Button_PGLoeschen']))
	{
		if(!isset($GLOBALS['fxptdata']['backup']['warnhinweis']))
		{
			$GLOBALS['fxptdata']['backup']['warnhinweis']=true;
			error_msg(2134, false, '<b>'.$Button_PGLoeschen_btxt.'</b>');
			$err=true;
		}
		else
			unset($GLOBALS['fxptdata']['backup']['warnhinweis']);
	}
	else if(isset($GLOBALS['fxptdata']['backup']['warnhinweis']))
		unset($GLOBALS['fxptdata']['backup']['warnhinweis']);
}
// ...Client's Contact Person (= Administrator)
// Administrators cannot be deleted! Only additional data can be deleted.
if($_pid && ((($_ptype == FXP_PT_CLIENT_CP) && ($ccc < 2)) || ($_spda['uid'] == 1)) && ($GLOBALS['fxpglobals']['action'] == FXP_DELETE) && fxIsArray($_POST) && !isset($_POST['Button_PGLoeschen']))
	error_msg(167, false);
// Only pictures, address and communication data can be deleted. The person or the company itself cannot be deleted, because the following $var1 person(s) are subordinated to it:$var2
if($_pid && ($_ptype != FXP_PT_CLIENT) && ($_ptype != FXP_PT_CLIENT_CP) && ($GLOBALS['fxpglobals']['action'] == FXP_DELETE) && fxIsArray($_POST) && !isset($_POST['Button_PGLoeschen']) && strlen((string)$_spda['sub']))
	error_msg(2388, false, '<b>'.substr_count($_spda['sub'], '<li>').'</b>', $_spda['sub']);

// Handle mask displays
// ...Select
if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
	$maske382_nv=true;
// ...Info
if(($GLOBALS['fxpglobals']['action'] != FXP_DISPLAY) || !$_cid || !$_pid || !fxIsArray($_POST) || !isset($_POST['Person_waehlen']))
	$maske387_nv=true;
// ...Client
if(($_ptype != FXP_PT_CLIENT) || ($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || (($GLOBALS['fxpglobals']['action'] > FXP_CREATE) && !$_pid))
	$maske394_nv=true;
// ...Person
if(($_ptype == FXP_PT_CLIENT) || ($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || (($GLOBALS['fxpglobals']['action'] > FXP_CREATE) && !$_pid))
	$maske383_nv=true;
// ...Cost Accounting
if(($_ptype == FXP_PT_CLIENT) || ($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || (($GLOBALS['fxpglobals']['action'] > FXP_CREATE) && !$_pid))
	$maske484_nv=true;
// ...Addresses/Communication
if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || (($GLOBALS['fxpglobals']['action'] > FXP_CREATE) && !$_pid))
{
	$maske384_nv=true;
	$maske386_nv=true;
}
?>