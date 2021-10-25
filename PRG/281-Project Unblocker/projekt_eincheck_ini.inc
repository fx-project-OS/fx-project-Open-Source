<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projekt_eincheck_ini.inc                                     //
// Version     : 21.2                                                         //
// Begin       : 2020-12-16                                                   //
// Last Change : 2020-12-16                                                   //
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
 * PF 281: Project Unblocker
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$as=substr($GLOBALS['datetime'],0,12).'00';

// Read projects log
$pp=prjLog('read');
$pl=array();
if(sizeof($pp))
{
	foreach($pp as $id => $d)
	{
		if($d['client'] == $GLOBALS['fxpglobals']['client'])
			$pl[$d['mpid']]=$d;
	}
}

// Status
if($GLOBALS['fxpglobals']['lang'] == 1)
{
	$sta=array(
		-2	=> 'Warten auf Statusänderung',
		-1	=> 'Aufhebungsanfrage senden',
		0	=> '<font class="red">Blockiert</font>',
		1	=> '<font class="darkgrey">Anfrage gesendet</font>',
		2	=> '<font class="green">Anfrage akzeptiert</font>',
		3	=> '<font class="blue">Anfrage zur Kenntnis genommen</font>',
		4	=> '<font class="red">Anfrage abgewiesen</font>',
		98	=> '<font class="bold green">Person abgemeldet</font>',
		99	=> '<font class="bold green">Blockade aufgehoben</font>'
	);
}
else
{
	$sta=array(
		-2	=> 'Waiting for status change',
		-1	=> 'Send lift request',
		0	=> '<font class="red">Blocked</font>',
		1	=> '<font class="darkgrey">Request sent</font>',
		2	=> '<font class="green">Request accepted</font>',
		3	=> '<font class="blue">Request acknowledged</font>',
		4	=> '<font class="red">Request rejected</font>',
		98	=> '<font class="bold green">Person logged out</font>',
		99	=> '<font class="bold green">Blockade lifted</font>'
	);
}
echo('<div id="pci_st" style="display:none;">'.$sta[-2].'|'.$sta[-1].'|'.$sta[0].'|'.$sta[1].'|'.$sta[2].'|'.$sta[3].'|'.$sta[4].'|'.$sta[98].'|'.$sta[99].'</div>'.$nl);

// Basic settings and checked out main projects list (from PF 20: Archive Projects)
$GLOBALS['prjCheckWarning']=false;
$req_fn=fxf_fn_reqFilename('projekt_archivieren_ini', 20);
if(strlen($req_fn))
	require($req_fn);

// List all checked out main projects
if(sizeof($mp_list))
{
//fxDebug($mp_list,'$mp_list');
	// Header
	$sql="SELECT text_kurz FROM texte WHERE id_referenz=5 AND id_ref_art=2 AND id_sprache=".$GLOBALS['fxpglobals']['lang'];
	$maske5_head=db_value($sql).' <font class="blue">('.get_entry($GLOBALS['datetime'],'datetime').')</font>';

	// Use list index
	$index=true;

	// Nowrap
	$Mutterprojekt_ID_nw=true;
	$Projektmanager_nw=true;
	$Projektleiter_nw=true;
	$cout_id_nw=true;
	$cout_date_nw=true;
	$Statusfeld_nw=true;
	$aktionscode_nw=true;

	// Frames
	$aktionscode_ffs=0;

	// Person list
	$ps_list=array();

	// Mask variables
	$mc=$GLOBALS['_maskcounter'];
	$z=0;

	foreach($mp_list as $mpid => $mp)
	{
		// Main Project [mutterprojekt_id]
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mutterprojekt_id'] = '~<font class="black"><b>';
		if($sort_number)
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mutterprojekt_id'] .= no_null($mp['vorgangsnummer']).'</b>&nbsp;&nbsp;'.$mp['name_projekt'];
		else
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mutterprojekt_id'] .= $mp['name_projekt'].'</b>&nbsp;&nbsp;['.no_null($mp['vorgangsnummer']).']';
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mutterprojekt_id'] .= '</font>';

		// Project manager [projektmanager]
		$psid=$mp['projektmanager'];
		if(!isset($ps_list[$psid]))
			$ps_list[$psid]=getPersonName($psid);
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projektmanager']='~<font class="darkgrey">'.$ps_list[$psid].'</font>';

		// Project leader [projektleiter]
		$psid=$mp['projektleiter'];
		if(!isset($ps_list[$psid]))
			$ps_list[$psid]=getPersonName($psid);
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projektleiter']='~<font class="darkgrey">'.$ps_list[$psid].'</font>';
		$psid2=$mp['vertreter'];
		if($psid2 && ($psid2 != $psid))
		{
			if(!isset($ps_list[$psid2]))
				$ps_list[$psid2]=getPersonName($psid2);
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projektleiter'] .= ' <font class="lightgrey">(+ '.$ps_list[$psid2].')</font>';
		}

		// Checked-Out by [cout_id]
		$psid=$mp['cout_id'];
		if(!isset($ps_list[$psid]))
			$ps_list[$psid]=getPersonName($psid);
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['cout_id']='~<b class="blue">'.$ps_list[$psid].'</b>';
		if(($psid == $GLOBALS['fxpglobals']['person']) && sizeof($pl) && isset($pl[$mpid]))
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['cout_id'] .= ' <i class="lightblue">('.substr($pl[$mpid]['clts'],8).')</i>';

		// Time [cout_date]
		if(sizeof($pl) && isset($pl[$mpid]))
			$cs=substr($pl[$mpid]['ccts'],0,12).'00';
		else
			$cs=substr($mp['cout_date'],0,12).'00';
		$ds=floor(time_diff($as,$cs,'N'));
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['cout_date']='~<b class="blue">'.get_entry($cs,'zeit').'</b> <i class="lightblue">('.$ds.' min)</i>';

		// Status [Statusfeld]
		$st=0;
		$sc='';
		if(sizeof($pl) && isset($pl[$mpid]))
		{
			$st=$pl[$mpid]['cnstatus'];
			$sc='<font class="lightgrey">'.get_entry($cs,'zeit').'</font> ';
		}
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['statusfeld']='~<span id="pci_smp_'.$mpid.'">'.$sc.$sta[$st].'</span>';

		// [aktionscode]
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['aktionscode']='^class_transparent|~&nbsp;&nbsp;<span id="pci_amp_'.$mpid.'">';
		if(!$st)
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['aktionscode'] .= '<span class="btn" '.fxf_jsFunction('keepUserAlive', $mpid.'|1').'>'.$sta[-1].'</span>';
		else if($st < 98)
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['aktionscode'] .= '<img src="'.$GLOBALS['gfxpath'].'uploading.gif" tooltip="'.$sta[-2].'&hellip;">';
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['aktionscode'] .= '</span><br /><br />';

		$z++;
	}

	// Last line with Top Border?
	if($z)
	{
		$fa=fxf_getMaskFields(5,false,array('aktionscode'=>0));
		fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
		$z++;
	}
}
?>