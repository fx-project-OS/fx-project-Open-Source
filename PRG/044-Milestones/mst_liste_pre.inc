<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : mst_liste_pre.inc                                            //
// Version     : 24.1                                                         //
// Begin       : 2021-01-15                                                   //
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
 * PF 44: Milestones
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

if(!isset($with_planned_milestones))
	$with_planned_milestones=true;

// Get date for the end of next month
$until=fx_date('Ymd240000', fx_mktime(12,0,0, (int)substr((string)$GLOBALS['datetime'],4,2)+2, 0, (int)substr((string)$GLOBALS['datetime'],0,4)));

// Headline
$maske543_head=get_text(543,'kurz',0,2).' <span class="s2 grey">('.meldung(FXP_PS_ACTIVE);
if($with_planned_milestones)
	$maske543_head .= ' + '.meldung(FXP_PS_PLANNED);
$maske543_head .= ')</span>';

// Prepare SQL
$sql  = "SELECT m.meilenstein_id, m.name_meilenstein, m.projekt_id, m.soll_ende, m.statusfeld, m.tagesanzahl, p.vorgangsnummer, p.name_projekt, p.projektart, p.projekt_status, z.soll_beginn AS psd, z.soll_ende AS ped";
$sql .= " FROM meilensteine m, projekte p, zeitdaten z";
$sql .= " WHERE p.projekt_id=m.projekt_id AND m.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND z.projekt_id=m.projekt_id AND z.zeitart=102 AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];
$sql .= " AND (p.projekt_status=".FXP_PS_ACTIVE;
if($with_planned_milestones)
	$sql .= " OR (p.projekt_status=".FXP_PS_PLANNED." AND m.soll_ende<='".$until."')";
$sql .= ")";
// ...Admin + GM can see all milestones, everybody else only milestone to projects they are assigned to
if(($GLOBALS['fxpglobals']['person'] > 1) && !$GLOBALS['fxpglobals']['persdat']['roles'][2])
{
	// Get actual persons's parent
	$has_parent=get_realelter();

	$sql .= " AND (p.projektmanager=".$GLOBALS['fxpglobals']['person']." OR p.projektleiter=".$GLOBALS['fxpglobals']['person']." OR p.vertreter=".$GLOBALS['fxpglobals']['person']." OR ansprechpartner=".$GLOBALS['fxpglobals']['person'];
	if($has_parent)
		$sql .= " OR kunde=".$has_parent;
	$sql .= ")";
}
$sql .= " ORDER BY m.soll_ende DESC, p.vorgangsnummer";

// Get data
$msta=db_values($sql,'*');
//fxDebug($msta, $sql);

// Show data
if(fxIsArray($msta))
{
	// Sort order
	$Datum_sort='DESC';

	// Literals
	$Soll_Ende_lit=get_text(273,'20');			// -> Date
	$zeitraumBalken_lit=get_text(1325,'15');	// -> Timespan

	// Nowrap
	$Statusfeld_nw=true;
	$Soll_Ende_nw=true;
	$Meilenstein_ID_nw=true;
	$typ_nw=true;
	$zeitraumBalken_nw=true;

	// Status Warn Level <i>(0-10)</i>|The status of the assigned project is: <b>planned</b>|The status of the assigned project is <b>active</b>|The milestone date is in the <b>Past</b>|The milestone date is <b>Today</b>|The milestone date is in the <b>Future</b>|The milestone is <b>dynamically</b> assigned to the start date of the project|The milestone is <b>fixed</b> assigned to the project|The milestone is <b>dynamically</b> assigned to the end date of the project|The assigned project date is in the <b>Past</b>|The assigned project date is <b>Today</b>|The assigned project date is in the <b>Future</b>
	$ma=explode('|', meldung(3206));
//fxDebug($ma,'$ma');

	// Variables
	$mc=$GLOBALS['_maskcounter'];
	$z=0;
	$t=false;

	$fa=fxf_getMaskFields(543);
//fxDebug($fa,'$fa', 0);

	foreach($msta as $a)
	{
		$md=substr((string)$a['soll_ende'],0,8);
		$ad=substr((string)$GLOBALS['datetime'],0,8);
		$ps=substr((string)$a['psd'],0,8);
		$pe=substr((string)$a['ped'],0,8);

		// Today?
		if(!$t && ($md < $ad))
		{
			if($z)
			{
				fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
				$z++;
			}

			$txt=get_text(540,'15').': '.get_entry($ad,'datum');
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['statusfeld']='^class_rhd" nowrap align="center" colspan="6"><!--|~'.$txt;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['zeitraumbalken']='~-->'.$txt;
			$z++;

			$t=true;
		}

		// Status
		$stt='';
		$swl=0;										// Status warn level
		$stn=$a['statusfeld'];						// ...milestone node: -1=start, 0=fix, 1=end
		$std=$md;									// ...node date
		if($stn < 0)
			$std=$ps;
		else if($stn > 0)
			$std=$pe;
		$sl=0;
		$ic='green';
		if($std < $ad)
		{
			$ic='yellow';
			$sl++;
			if($pe < $ad)
			{
				$ic='red';
				$sl++;
			}
		}
		$swl += $sl;
		if($stn < 0)
			$stt .= '<img src='.$GLOBALS['gfxpath'].'st_'.$ic.'.png style=vertical-align:middle;> ['.$sl.'] - '.$ma[6].': <b>'.get_entry($std,'datum').'</b><br />';
		else if($stn > 0)
			$stt .= '<img src='.$GLOBALS['gfxpath'].'st_'.$ic.'.png style=vertical-align:middle;> ['.$sl.'] - '.$ma[8].': <b>'.get_entry($std,'datum').'</b><br />';
		else
			$stt .= '<img src='.$GLOBALS['gfxpath'].'st_'.$ic.'.png style=vertical-align:middle;> ['.$sl.'] - '.$ma[7].': <b>'.get_entry($std,'datum').'</b><br />';

		$stm=0;										// ...milestone date: -1=past, 0=today, 1=future
		$sl=0;
		if($md < $ad)
		{
			$stm=-1;
			$sl += 2;
			$stt .= '<img src='.$GLOBALS['gfxpath'].'st_red.png style=vertical-align:middle;> ['.$sl.'] - '.$ma[3].'<br />';
		}
		else if($md > $ad)
		{
			$stm=1;
			$stt .= '<img src='.$GLOBALS['gfxpath'].'st_green.png style=vertical-align:middle;> ['.$sl.'] - '.$ma[5].'<br />';
		}
		else
		{
			$sl++;
			$stt .= '<img src='.$GLOBALS['gfxpath'].'st_yellow.png style=vertical-align:middle;> ['.$sl.'] - '.$ma[4].'<br />';
		}
		$swl += $sl;

		$sts=0;										// ...project status: 0=active, 1=planned
		$sl=0;
		if($a['projekt_status'] == FXP_PS_ACTIVE)
		{
			$sts=1;
			$stt .= '<img src='.$GLOBALS['gfxpath'].'st_green.png style=vertical-align:middle;> ['.$sl.'] - '.$ma[2].'<br />';
		}
		else
		{
				$ic='yellow';
				$sl++;
				if($ps < $ad)
				{
					$sl += 3;
					$ic='red';
				}
			$stt .= '<img src='.$GLOBALS['gfxpath'].'st_'.$ic.'.png style=vertical-align:middle;> ['.$sl.'] - '.$ma[1].'<br />';
		}
		$swl += $sl;

		$sl=0;
		if($std < $ad)
		{
			$sl += 2;
			$stt .= '<img src='.$GLOBALS['gfxpath'].'st_red.png style=vertical-align:middle;> ['.$sl.'] - '.$ma[9].'<br />';
		}
		else if($std > $ad)
			$stt .= '<img src='.$GLOBALS['gfxpath'].'st_green.png style=vertical-align:middle;> ['.$sl.'] - '.$ma[11].'<br />';
		else
		{
			$sl++;
			$stt .= '<img src='.$GLOBALS['gfxpath'].'st_yellow.png style=vertical-align:middle;> ['.$sl.'] - '.$ma[10].'<br />';
		}
		$swl += $sl;

		if($swl < 3)
		{
			$sc='green';
			$fc='black';
		}
		else if($swl < 6)
		{
			$sc='yellow';
			$fc='grey';
		}
		else
		{
			$sc='red';
			$fc='red';
		}
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['statusfeld']='~<div tooltip="<div style=\'padding:4px;background:#eeeeee;border-bottom:1px solid black;\'>'.$ma[0].': <b class='.$fc.'>'.$swl.'</b></div><br />'.$stt.'" style="height:16px;cursor:help;"><img src="'.$GLOBALS['gfxpath'].'tl_'.$sc.'.png" style="float:left;margin-top:2px;margin-left:4px;"><div class="'.$fc.'" style="float:left;text-align:right;width:22px;">'.$swl.'</div></div>';
		// Date
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['soll_ende']='~<b class="'.$fc.'">'.get_entry($md,'datum').'</b>';
		// Milestone
		$mi='mst';
		if(!$a['statusfeld'])
			$mi .= 'f';
		if($a['projekt_status'] == FXP_PS_ACTIVE)
			$mn=$a['name_meilenstein'];
		else
			$mn='<i>'.$a['name_meilenstein'].'</i>';
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['meilenstein_id']='~<img src="'.$GLOBALS['gfxpath'].$mi.'.png" align="top" width="18" height="18" style="margin-right:4px;">'.$mn;
		// Type
		$mt='<b class="darkgrey">';
		if($a['statusfeld'] < 0)
			$mt .= 'S';
		else if($a['statusfeld'] > 0)
			$mt .= 'E';
		else
			$mt .= 'F';
		$mt .= '</b>';
		if($a['tagesanzahl'])
		{
			if($a['tagesanzahl'] > 0)
				$a['tagesanzahl']='+'.$a['tagesanzahl'];
			$mt .= '&nbsp;<i class="grey">('.$a['tagesanzahl'].')</i>';
		}
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['typ']='~'.$mt;
		// Assigned to Object
		$pc='lightgrey';
		if($a['projektart'] <= FXP_PRJ_TASK)
			$pc='grey';
		$pt='';
		$pn=$a['name_projekt'];
		if(strlen((string)$pn) > 32)
		{
			$pt=' tooltip="'.str_replace('"', '&quot;', $pn).'" style="cursor:help;"';
			$pn=trim(substr((string)$pn,0,32)).'...&nbsp;<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top">';
		}
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['elter']='~<img src="'.$GLOBALS['gfxpath'].'lt'.$a['projekt_status'].'.png" align="top" style="margin-right:4px;"><b class="'.$pc.'">'.no_null($a['vorgangsnummer']).'</b>&nbsp;&nbsp;<span class="'.$pc.'"'.$pt.'>'.$pn.'</span>';
		// Timespan
		$ts=get_entry($ps,'datum');
		if($pe != $ps)
			$ts .= '-'.get_entry($pe,'datum');
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['zeitraumbalken']='~<span class="lightgrey">'.$ts.'</span>';

		$z++;
	}

	if($z)
	{
		fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
		$z++;
	}
}
?>