<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : faezuordnung_pre.inc                                         //
// Version     : 21.1                                                         //
// Begin       : 2020-10-20                                                   //
// Last Change : 2020-10-20                                                   //
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
 * PF 22: Employee's Roles
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$index=true;
$mc=$GLOBALS['_maskcounter'];
$pid=(int)$Personen_ID;

// Nowrap
$MaFaehigkeit_Name_nw=true;
$Dauer_nw=true;
$Bewertung_nw=true;
$Name_Benutzer_nw=true;

// Skill list
$sla=array();
// 1. Step: Get all skill groups
$sql  = "SELECT mandanten_id, maart_id, maart_name";
$sql .= " FROM ma_art";
$sql .= " WHERE mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].") AND id_sprache=".$GLOBALS['fxpglobals']['lang'];
if(($GLOBALS['fxpglobals']['tr'] != 22) && ($GLOBALS['fxpglobals']['tr'] != 24))
	$sql .= " AND maart_id NOT IN (1,2)";
else
{
	$ag='2';
	// ...Only administrators, general managers or administrative personal should handle employee roles
	if(!$GLOBALS['fxpglobals']['person'] || ($GLOBALS['fxpglobals']['user'] <= 1) || $GLOBALS['fxpglobals']['useradmin'] || $GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2])
		$ag .= ',1';
	$sql .= " AND maart_id IN (".$ag.")";
}
$sql .= " ".db_orderby('maart_name');
$tmp=db_values($sql,'*');
fxDebug($tmp,$sql, 3);
if(fxIsArray($tmp))
{
	foreach($tmp as $t)
		$sla[(int)$t['maart_id']]=array('name'=>$t['maart_name'], 'client'=>(int)$t['mandanten_id'], 'skills'=>array());

	// Check, if "Funded project management" should be used
	$fpm=false;
	// ...yes, if a person has the skill already
	$sql="SELECT ma_art_faehigkeit_id FROM ma_art_faehigkeiten WHERE mafaehigkeit_id=-5 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$chk=(int)db_value($sql);
fxDebug($chk,$sql, 3);
	if($chk)
		$fpm=true;
	else
	{
		// ...yes, if a program function (272-279) has been assigned already to a profile
		$sql="SELECT COUNT(rechte_trans_id) FROM rechte_trans WHERE trans_id>271 AND trans_id<280 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$chk=(int)db_value($sql);
fxDebug($chk,$sql, 3);
		if($chk)
			$fpm=true;
	}

	// 2. Step: Get all skills
	$sql  = "SELECT mandanten_id, maart_id, mafaehigkeit_id, mafaehigkeit_name";
	$sql .= " FROM ma_faehigkeit";
	$sql .= " WHERE mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].") AND id_sprache=".$GLOBALS['fxpglobals']['lang'];
	if(!$fpm)
		$sql .= " AND mafaehigkeit_id<>-5";
	$sql .= " ".db_orderby('mafaehigkeit_name');
	$tmp=db_values($sql,'*');
fxDebug($tmp,$sql, 3);
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
		{
			$sg=(int)$t['maart_id'];
			if(isset($sla[$sg]))
				$sla[$sg]['skills'][(int)$t['mafaehigkeit_id']]=array('name'=>$t['mafaehigkeit_name'], 'client'=>$t['mandanten_id'], 'id'=>0, 'experience'=>"", 'rating'=>0, 'comment'=>"", 'changed'=>"");
		}
	}

	// 3. Step: Get person skills
	if($pid)
	{
		$sql  = "SELECT ma_art_faehigkeit_id, maart_id, mafaehigkeit_id, dauer, kommentar, bewertung, aenderungs_id, zeitstempel";
		$sql .= " FROM ma_art_faehigkeiten";
		$sql .= " WHERE personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$tmp=db_values($sql,'*');
fxDebug($tmp,$sql, 3);
		if(fxIsArray($tmp))
		{
			$ca=array();
			foreach($tmp as $t)
			{
				$sg=(int)$t['maart_id'];
				$sk=(int)$t['mafaehigkeit_id'];
				if(isset($sla[$sg]) && isset($sla[$sg]['skills'][$sk]))
				{
					$ci=(int)$t['aenderungs_id'];
					if(!isset($ca[$ci]))
					{
						$nta=db_values("SELECT pname,vorname FROM personen WHERE personen_id=".$ci." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
						if(fxIsArray($nta))
						{
							$n=trim($nta['pname']);
							if(strlen(trim($nta['vorname'])))
								$n .= ',&nbsp;'.trim($nta['vorname']);
						}
						else
							$n='???';
						$ca[$ci]=$n;
					}

					$sla[$sg]['skills'][$sk]['id']			= (int)$t['ma_art_faehigkeit_id'];
					$sla[$sg]['skills'][$sk]['experience']	= $t['dauer'];
					$sla[$sg]['skills'][$sk]['rating']		= $t['bewertung'];
					$sla[$sg]['skills'][$sk]['comment']		= $t['kommentar'];
					$sla[$sg]['skills'][$sk]['changed']		= '<table border=0 cellpadding=0 cellspacing=0><tr><td nowrap><font class="darkgrey">'.$ca[$ci].'</font></td></tr><tr><td nowrap><i class="s2 grey">('.get_entry($t['zeitstempel'],'datetime').')</i></td></tr></table>';
				}
			}
		}
	}
}
fxDebug($sla,'$sla', 2);

// Populate list
$GLOBALS['fxptdata']['lvalues'][$mc]=array();
if(sizeof($sla))
{
	$ems=meldung(794);				// empty
	$dea=explode(',', meldung(16));	// Display,Create,Edit,Delete,Add
	$des=$dea[FXP_DELETE-1];		// ...Delete
	$ndm=meldung(3062);				// This role cannot be deleted anymore, because this person<br />is already assigned to at least one project in this role!
	$Aenderungs_Zeit_lit=get_text(677,'10').'/'.get_text(545,'10');
	$l=0;

	// Is this person a project manager or a project leader?
	$ppm=false;
	$ppl=false;
	if($pid)
	{
		$sql="SELECT projekt_id FROM projekte WHERE projektmanager=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$prj=db_value($sql);
		if($prj)
			$ppm=true;
		$sql="SELECT projekt_id FROM projekte WHERE (projektleiter=".$pid." OR vertreter=".$pid.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$prj=db_value($sql);
		if($prj)
			$ppl=true;
	}
fxDebug($ppm,'$ppm: $pid='.$pid, 3);
fxDebug($ppl,'$ppl: $pid='.$pid, 3);

	// No border for this column
	fxf_maskColumnBorder(array('Checkbox'), 0);

	foreach($sla as $gi => $ga)
	{
		// Divider with top border
		if($l)
		{
			fxf_maskColumnBorder(array('Checkbox', 'MaFaehigkeit_Name', 'Dauer', 'Bewertung', 'Kommentar', 'Name_Benutzer'), 1, -1, $mc, $l, true);
			$l++;
		}

		// Group
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['checkbox']='~|';
		fxf_listSectHeader($mc, $l, '&nbsp;<font class="s4b">'.$ga['name'].'</font>', 'checkbox', 'name_benutzer', 6, -2, 1);
		$l++;

		// Skills
		if(sizeof($ga['skills']))
		{
			foreach($ga['skills'] as $si => $sa)
			{
fxDebug($sa,$si, 3);
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]=array();

				// Class/Tooltips
				$cls='';
				$ttl='';
				$tti='';
				if($sa['id'])
					$cls .= 'bold ';
				if($err && isset($ea[$l]))
					$cls .= 'red';
				else if($sa['id'])
				{
					if(($ppm && ($si == -2)) || ($ppl && ($si == 1)))
					{
						$cls .= 'blue';
						$ttl=' tooltip="'.$ndm.'" style="cursor:help;"';
						$tti=' <img class="op33" src="'.$GLOBALS['gfxpath'].'hlp_b_21x21.png" align="top" style="float:right;">';
					}
					else
						$cls .= 'black';
				}
				else
					$cls .= 'grey';

				// ...[Checkbox] {Checkbox}
				if(!$sa['id'] || strlen($tti))
					$GLOBALS['fxptdata']['lvalues'][$mc][$l]['checkbox']='~';
				else
				{
					${'Checkbox'.$l.'_tt'}=$des;
					if($err)
						$GLOBALS['fxptdata']['lvalues'][$mc][$l]['checkbox']=$_POST['Checkbox'][$mc][$l];
				}

				// ...Fähigkeitsname [MaFaehigkeit_Name] {Read Only}
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['mafaehigkeit_name']='~<span class="'.$cls.'"'.$ttl.'>'.$sa['name'].$tti.'</span>';

				// ...Erf. [Dauer] {Text}
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['dauer']=$sa['experience'];
				if($err)
					$GLOBALS['fxptdata']['lvalues'][$mc][$l]['dauer']=$_POST['Dauer'][$mc][$l];

				// ...Bew. [Bewertung] {Select}
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['bewertung']=$sa['rating'];
				if($err)
					$GLOBALS['fxptdata']['lvalues'][$mc][$l]['bewertung']=$_POST['Bewertung'][$mc][$l];

				// ...Kommentar [Kommentar] {Textarea}
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['kommentar']=$sa['comment'];
				if($err)
					$GLOBALS['fxptdata']['lvalues'][$mc][$l]['kommentar']=$_POST['Kommentar'][$mc][$l];

				// ...Erstellt/Geändert [Name_Benutzer] {Read Only}
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['name_benutzer']='~'.$sa['changed'];

				// ...Hidden
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['maart_id']=$gi;
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['mafaehigkeit_id']=$si;
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['dummy']=$sa['id'].'|'.$sa['experience'].'|'.$sa['rating'].'|'.$sa['comment'].'|'.$ga['name'].'|'.$sa['name'];

				$l++;
			}
		}
		else
		{
			$txt='<i class="s2 lightgrey">&nbsp;&nbsp;('.$ems.')</i>';
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]=array();

			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['mafaehigkeit_name']='^class_?" nowrap colspan="6"><!--|~'.$txt;
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['checkbox']='~-->'.$txt;

			$l++;
		}
	}

	// Top border
	if($l)
	{
		fxf_maskColumnBorder(array('Checkbox', 'MaFaehigkeit_Name', 'Dauer', 'Bewertung', 'Kommentar', 'Name_Benutzer'), 1, -1, $mc, $l, true);
		$l++;
	}
}
fxDebug($GLOBALS['fxptdata']['lvalues'][$mc],'$GLOBALS[\'fxptdata\'][\'lvalues\']['.$mc.']', 3);
?>