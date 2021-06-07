<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : userprefs_pre.inc                                            //
// Version     : 21.1                                                         //
// Begin       : 2020-09-22                                                   //
// Last Change : 2020-10-15                                                   //
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
 * PF 25: User Settings - File for presetting the mask fields
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

// Mask prepearation variables
$index=true;
$mc=$GLOBALS['_maskcounter'];
setForm(true);

$GLOBALS['fxptdata']['lvalues'][$mc]=array();
$GLOBALS['fxptdata']['litf_138']['Loeschen']=get_text(118,'20');	// Delete (Display as headline text in checkbox popup)

// No right border in the following columns
fxf_maskColumnBorder(array('Loeschen', 'nv_text1', 'icon'), 0, -1, $mc);

// Select/Input field width in pixel
$fldwd=420;

// Get current user settings
$usr_settings=benutzereinstellungen($GLOBALS['fxpglobals']['user'], false, false);
fxDebug($usr_settings, '$usr_settings', 3);

// Get all possible groups
$grp=array();
$sql  = "SELECT wertetabid, tabwert FROM wertetab";
$sql .= " WHERE mandanten_id=0 AND id_feld=2 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND (archiv=0 OR archiv IS NULL)";
$sql .= " ORDER BY tabwert";
$tmp=db_values($sql, '*');
if(fxIsArray($tmp))
{
	foreach($tmp as $t)
		$grp[(int)$t['wertetabid']]=trim($t['tabwert']);
}
$grp[0]=meldung(1449, false);	// [Unknown]
fxDebug($grp, '$grp: '.$sql, 2);

// Get all setting texts (except unused settings)
$uus='';
if(!tr_program(241))	// No "Cost Accounting"?
{
	if(strlen($uus))
		$uus .= ", ";
	$uus .= "802,803,921,1084,1085,1086,1087";
}
$txt=array();
if(fxIsArray($grp))
{
	$sql  = "SELECT wertetabid, tabwert, position FROM wertetab";
	$sql .= " WHERE mandanten_id=0 AND id_feld=253 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND (archiv=0 OR archiv IS NULL)";
	if(strlen($uus))
		$sql .= " AND wertetabid NOT IN (".$uus.")";
	$sql .= " ORDER BY tabwert";
	$tmp=db_values($sql, '*');
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
			$txt[(int)$t['wertetabid']]=array('grp'=>(int)substr($t['position'], -4), 'txt'=>trim($t['tabwert']));
	}
fxDebug($txt, '$txt: '.$sql, 2);
}

// Get all setting values
$set=array();
if(fxIsArray($txt))
{
	$sql  = "SELECT ed.einstellungs_id, ed.einstellungs_index, ed.einstellungs_name, ed.id_feld,";
	$sql .= " ev.vergaberecht_id, ev.vergaberecht,";
	$sql .= " ew.einstellungswerte_id, ew.einstellungswert";
	$sql .= " FROM s_einstellungsdef ed, s_einstellungsrecht ev, s_einstellungswerte ew";
	$sql .= " WHERE ev.einstellungs_id=ed.einstellungs_id AND ev.mandanten_id=0 AND ew.einstellungs_id=ed.einstellungs_id AND ew.mandanten_id=0 AND ew.benutzer_id=1";
	$set=db_values($sql, '*');
fxDebug($set, '$set: '.$sql, 2);
}

// Get all client setting values
$mset=array();
if(fxIsArray($txt))
{
	$sql  = "SELECT einstellungs_id, einstellungswerte_id, einstellungswert";
	$sql .= " FROM s_einstellungswerte";
	$sql .= " WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND benutzer_id=";
	// ...Client or user settings?
	if(($tr == 259) || ($tr == 269))
		$sql .= "1";
	else
		$sql .= $GLOBALS['fxpglobals']['user'];
	$tmp=db_values($sql, '*');
fxDebug($tmp, $sql, 3);
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
			$mset[$t['einstellungs_id']]=array('id'=>$t['einstellungswerte_id'], 'value'=>$t['einstellungswert']);
	}
}
fxDebug($mset, '$mset', 2);

// Prepeare list and overwrite special settings
if(fxIsArray($set))
{
	// Get "lookup" setting texts
	$ncsv='';
	foreach($set as $s)
	{
		$sid=(int)$s['id_feld'];
		if(!$sid)
			continue;
		if(strlen($ncsv))
			$ncsv .= ',';
		$ncsv .= $sid;
	}
	$ntxt=array();
	if(strlen($ncsv))
	{
		$sql  = "SELECT id_feld, wertetabid, tabwert FROM wertetab";
		$sql .= " WHERE id_feld IN (".$ncsv.") AND id_feld<>53 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND (mandanten_id=0 OR mandanten_id=".$GLOBALS['fxpglobals']['client'].") AND (archiv=0 OR archiv IS NULL)";
		$sql .= " ORDER BY position, tabwert";
		$tmp=db_values($sql, '*');
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				$fid=(int)$t['id_feld'];
				$wid=(int)$t['wertetabid'];

				if(!fxIsArray($ntxt[$fid]))
				{
					$ntxt[$fid]=array();
					if($fid == 330)	// Person Unit: Unit for project effort fields
						$ntxt[$fid][0]=meldung(3054);	// (Unit can be set individually by every person)
				}
				if(($fid != 330) || ($wid != 856))	// Person Unit: No "Min."
					$ntxt[$fid][$wid]=trim($t['tabwert']);
			}
		}
	}
fxDebug($ntxt, '$ntxt: '.$sql, 2);

	// Preset variables
	$wta=explode('|',meldung(3341,false));	// Default value|Changed default value|User value
fxDebug($wta,'$wta', 3);
	$cuv_tt=meldung(1033,false);			// Delete this set <b>User value</b>?<br /><br /><i class=grey>(If you delete this value, it will be replaced with the initial default value!)</i>
	$help_tt=meldung(703,false);			// Show help for setting
	$z=0;

	// Display list arranged in group sections
	foreach($grp as $grp_id => $grp_txt)
	{
		$grp_shown=false;
		foreach($txt as $txt_id => $txt_array)
		{
			// Skip settings not in current group
			$act_grp_id=(int)$txt_array['grp'];
			if(!isset($grp[$act_grp_id]))
				$act_grp_id=0;
			if($act_grp_id != $grp_id)
				continue;

			foreach($set as $sn => $set_array)
			{
				$set_idx=$set_array['einstellungs_index'];

				$id=(int)$set_array['einstellungs_id'];
				$sid=(int)$set_array['id_feld'];
				$vr=(int)$set_array['vergaberecht'];

				$del_val='';
				$def_val=trim($set_array['einstellungswert']);
				$usr_val=trim($usr_settings[$set_idx]);

				// Really skip settings not in current group
				$act_name_id=(int)$set_array['einstellungs_name'];
				if($act_name_id != $txt_id)
					continue;
//echo('$grp_id='.$grp_id.', $act_grp_id='.$act_grp_id.', $set_idx='.$set_idx.', $id='.$id.', $sid='.$sid.', $act_name_id='.$act_name_id.', $txt_id='.$txt_id.'<br />');

				// Adjust integer and float values
				$def_type='string';
				if(is_float($usr_settings[$set_idx]))
				{
					$usr_val=get_entry($usr_val, 'dezimal');
					$def_type='float';
				}
				else if(is_integer($usr_settings[$set_idx]))
				{
					$usr_val=(int)$usr_val;
					$def_type='integer';
				}
//echo('<b>'.$set_idx.':</b> $def_val='.$def_val.' - $usr_val='.$usr_val.'<br />');

				// PF 25: User Settings - Only display these settings, skip everything else
				if(($tr == 25) && ($vr < 3))
					continue;
				// PF 259: Client Settings - Only display these settings, skip everything else
				if(($tr == 259) && ($vr > 2))
					continue;
				// PF 269: Allowances Mileage+Meals - Only display these settings, skip everything else
				if(($tr != 269) && (($act_grp_id == 1967) || ($act_grp_id == 1968)))
					continue;
				if(($tr == 269) && ($act_grp_id != 1967) && ($act_grp_id != 1968))
					continue;

				// Skip settings that the user is not allowed to change
				$chg=false;
				// ...Sysadmin
				if(!$GLOBALS['fxpglobals']['person'])
					$chg=true;
				// ...Admin or Admin 1
				else if($vr && (($GLOBALS['fxpglobals']['user'] == 1) || $usr_settings['b_admin']) && (($GLOBALS['fxpglobals']['client'] == 1) || ($vr > 1)))
					$chg=true;
				// ...User
				else if($vr > 2)
					$chg=true;
				if(!$chg)
					continue;

				// Skip language setting, because one can already set it by clicking on the flag icon in the taskbar
				if($id == 1)
					continue;

				// Reset user values on error
				if($err)
				{
					if(fxIsArray($_POST['loeschen']) && $_POST['loeschen'][$id])
						$del_val='1';
					if(strlen($_POST['newval'][$id]))
						$usr_val=$_POST['newval'][$id];
				}

				// Use text instead of ID for "lookup" values
				if($sid && fxIsArray($ntxt) && fxIsArray($ntxt[$sid]))
				{
					$def_val=$ntxt[$sid][$def_val];
					$usr_val=$ntxt[$sid][$usr_val];
				}

				// Display group?
				if(!$grp_shown)
				{
					$grp_shown=true;

					// Empty line with top border?
					if($z)
					{
						fxf_maskColumnBorder(array('Loeschen', 'nv_text1', 'preference', 'Einstellungswert', 'icon|0'), 1, -1, $mc, $z, true);
						$z++;
					}

					// Group header
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['icon']='~|';
					fxf_listSectHeader($mc, $z, '&nbsp;<font class="s4b">'.$grp_txt.'</font>', 'loeschen', 'einstellungswert', 4, -2, 1);
					$z++;
				}

				// Determine display classes
				$wtn=0;
				$cls='grey';
				$wtc='&uarr;';
				if(isset($mset[$id]))
				{
					$wtn=2;
					$cls='black';
					$wtc='&darr;';
				}
				if(!$vr)
					$cls='blue';	// Sysadmin settings

				// Display setting
				// Delete (with tooltip)
				if($wtn)
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['loeschen']='~#'.$cuv_tt.'|~'.fieldCheckbox('Loeschen['.$id.']', '1', $del_val);
				else
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['loeschen']='~';

				// Info (with tooltip)
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']='~#<span class='.$cls.'>'.$wtc.' = '.$wta[$wtn].'</span>|~&nbsp;&nbsp;&nbsp;<span class="'.$cls.'">'.$wtc.'</span>&nbsp;';

				// Help (with tooltip: Show help for setting...)
				$icon='~'.fieldLinkImage('icon_'.$z, 'hlp_b_21x21', 'loader.php?url=helptext.inc&hid=ew'.$id, $help_tt.' #<b>'.$grp_id.'-'.$txt_id.'-'.$id.'</b>', '', 'margin-left:8px;', 'op50');
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['icon']=$icon;

				// Setting text
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['preference']='~<font class="'.$cls.'">'.text_wrap($txt_array['txt'], true, 64).'</font>';

				// Setting value
				// ..."lookup"
				if($sid && fxIsArray($ntxt) && fxIsArray($ntxt[$sid]))
				{
					$skey=0;
					foreach($ntxt[$sid] as $opt_id => $opt_txt)
					{
						if($usr_val == $opt_txt)
						{
							$skey=$opt_id;
							break;
						}
					}
					$GLOBALS['newval'.$id.'_ds_array']=$ntxt[$sid];
					$val=fieldSelect('newval['.$id.']', $skey, $fldwd, 0, true);
				}
				// ...Input
				else if($tr == 269)	// Allowances Mileage+Meals
				{
					$unit=$GLOBALS['fxpglobals']['settings']['waehrung'];
					if(($id == 102) || ($id == 103))	// Shortages in percent
						$unit='%';
					$val='<div style="width:130px;">'.fieldText('newval['.$id.']', $usr_val, 100, 250, 'right', true, false, false, '', '', 'text', '', 'padding:3px;').'&nbsp;'.$unit.'</div>';
				}
				else
					$val=fieldText('newval['.$id.']', $usr_val, $fldwd, 250, 'left', true, false, false, '', '', 'text', '', 'padding:3px;');
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['einstellungswert']='~'.$val;

				// ...Hidden
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['einstellungswert'] .= fieldHidden('vr['.$id.']',$vr);									// [vr]		Right
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['einstellungswert'] .= fieldHidden('oldval['.$id.']',trim($usr_settings[$set_idx]));	// [oldval]	Old value
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['einstellungswert'] .= fieldHidden('typ['.$id.']',$def_type);							// [typ]	Type
				$z++;
			}
		}
	}

	// Empty line with top border?
	if($z)
	{
		fxf_maskColumnBorder(array('Loeschen', 'nv_text1', 'preference', 'Einstellungswert', 'icon|0'), 1, -1, $mc, $z, true);
		$z++;
	}
}

// No admin rights?
if(!sizeof($GLOBALS['fxptdata']['lvalues'][$mc]))
{
	$maske138_nv=true;
	// 3127: Sadly you don't have administration rights and are therefore not allowed to change client settings.
	$msg='<div class="s4 red" style="border:1px solid #e1001a;padding:4px;border-radius:6px;background:#fff0f4;">'.meldung(3127, false).'</div>';
	if(tr_rechte(30))
	{
		// 3128: To obtain administration rights the administrator flag has to be set in your user account. You can do this in the following program function
		$msg .= '<br />'.meldung(3128, false).':</font><br /><br />'.fxpLink(30);
	}
	echo('<div class="maskc" style="padding:12px;width:932px;">'.$msg.'</div><br />');
}
?>