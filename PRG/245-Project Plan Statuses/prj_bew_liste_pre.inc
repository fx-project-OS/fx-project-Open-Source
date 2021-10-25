<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prj_bew_liste_pre.inc                                        //
// Version     : 21.2                                                         //
// Begin       : 2021-01-14                                                   //
// Last Change : 2021-01-14                                                   //
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
 * PF 245: Project Plan Statuses
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

if(!isset($index))
	$index=true;

$pids=inferior_vn($pid);
//echo('<B><U>$pids:</U></B><BR>'.$pids.'<HR>');

// Hauptprojektinfos einlesen
$prj=array();
$sql  = "SELECT";
$sql .= " p.projekt_id, p.mutterprojekt_id, p.vorgangsnummer, p.name_projekt, p.projektart, p.elter, p.projekt_status, p.prioritaet,";
$sql .= " p.aufwand_soll, p.aufwand_ist, p.deadline, p.max_zeit_aufw,";
$sql .= " z.soll_beginn, z.soll_ende, z.ist_beginn, z.ist_ende";
$sql .= " FROM projekte p, zeitdaten z";
$sql .= " WHERE p.projekt_id IN (".$pids.") AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND z.zeit_id=p.zeit_id AND z.zeitart=102 AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];
$sql .= " ORDER BY p.vorgangsnummer";
$tmp=db_values($sql,'*');
fxDebug($tmp, $sql, 3);
if(fxIsArray($tmp))
{
	foreach($tmp as $t)
	{
		if(!$t['aufwand_soll'])
			$t['aufwand_soll']=0;
		if(!$t['aufwand_ist'])
			$t['aufwand_ist']=0;
		$prj[$t['projekt_id']]=$t;
	}
}
//fxDebug($prj,$sql, 0);

// Liste anzeigen
if(fxIsArray($prj))
{
	if($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)
	{
		$felder=array(
			'Projekt', 'Projekt_Status', 'Soll_Beg_Dtm', 'Soll_End_Dtm', 'Tagesanzahl',
			'nv_zahl1', 'nv_text1', 'nv_zahl2', 'nv_zahl3', 'nv_zahl4', 'Fertigstellung'
		);
	}
	else
	{
		$felder=array(
			'Projekt', 'Projekt_Status', 'Soll_Beg_Dtm', 'Soll_End_Dtm', 'Tagesanzahl',
			'SOLLStunden', 'nv_text1', 'ISTStunden', 'produktiv', 'Ver_Std', 'Fertigstellung'
		);
	}

	// Literal anpassen
	$Ver_Std_lit=get_text(1950,'10');
	$Ver_Std_lit_tooltip=get_text(1950,'15');

	// No-Wrap-Felder
	$lfelder=array();
	foreach($felder as $fc => $fn)
	{
		$lfelder[$fc]=strtolower($fn);
		${$fn.'_nw'}=true;
	}

	// Status + Monatsnamen laden
	$status_text=fxf_getReferenceValues('projekt_status');
	$monate_text=fxf_getReferenceValues('monate');

	// Historiendaten defaultmäßig auf 0.00 setzen
	$ehm=array();
	foreach($prj as $id => $p)
	{
		$cd=substr($GLOBALS['datetime'],0,6);
		$ehm[$id]=$cd;
		$sd=substr($p['soll_beginn'],0,6);
		$ib=substr($p['ist_beginn'],0,6);
		if($ib && ($ib < $sd))
			$sd=$ib;
		while(true)
		{
			$prj[$id]['hist'][$cd]=array('s'=>$p['aufwand_soll'], 'i'=>0.00, 'p'=>0.00, 'r'=>0.00);
			if($cd > $sd)
			{
				$y=(int)substr($cd,0,4);
				$m=(int)substr($cd,4,2) - 1;
				if($m < 1)
				{
					$y--;
					$m=12;
				}
				$cd=substr('0000'.$y,-4).substr('00'.$m,-2);
			}
			else
				break;
		}
	}
//fxDebug($prj,'$prj', 0);

	// Alle historischen Soll-Aufwände ermitteln
	$sql  = "SELECT projekt_id, aufwand_soll, zeitstempel";
	$sql .= " FROM projekte_history";
	$sql .= " WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " ORDER BY zeitstempel";
	$tmp=db_values($sql, '*');
//fxDebug($tmp,$sql, 0);
	if(fxIsArray($tmp))
	{
		$ad=substr($GLOBALS['datetime'],0,6);
		foreach($tmp as $t)
		{
			$id=$t['projekt_id'];
			$cd=substr($t['zeitstempel'],0,6);
			if((int)$cd < (int)$ehm[$id])
				$ehm[$id]=$cd;
			while(isset($prj[$id]['hist'][$cd]))
			{
				if($cd < $ad)
					$prj[$id]['hist'][$cd]['s']=$t['aufwand_soll'];
				$y=(int)substr($cd,0,4);
				$m=(int)substr($cd,4,2) + 1;
				if($m > 12)
				{
					$y++;
					$m=1;
				}
				$cd=substr('0000'.$y,-4).substr('00'.$m,-2);
			}
		}
	}
//fxDebug($ehm,'$ehm');

	// Alle produktiven Leistungsarten ermitteln
	$pla=array();
	$sql  = "SELECT z.zeit_id, z.projekt_id, z.zeitaufwand, z.ist_beginn, k.kostenstelle_id, s.kostennummer, s.kostenbezeichnung";
	$sql .= " FROM zeitdaten z, kostenstellen s, kostenzuordnung k";
	$sql .= " WHERE z.zeit_id=k.id AND s.kostenstelle_id=k.kostenstelle_id";
	$sql .= " AND k.kostenstellenart=".FXP_TE_TIMEREC." AND k.kostendefinition=903 AND k.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " AND z.projekt_id IN (".$pids.") AND z.zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.") AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " AND s.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND s.kostendefinition=903 AND (s.produktiv IS NOT NULL OR s.produktiv<>0) AND s.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=db_values($sql, '*');
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
			$pla[$t['zeit_id']]=$t;
	}
//fxDebug($pla,$sql, 0);

	// Alle Zeiterfassungswerte ermitteln
	$sql  = "SELECT zeit_id, projekt_id, zeitaufwand, ist_beginn, rechnungs_id";
	$sql .= " FROM zeitdaten";
	$sql .= " WHERE projekt_id IN (".$pids.") AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=db_values($sql, '*');
	if(fxIsArray($tmp))
	{
//fxDebug($tmp,$sql, 0);
		foreach($tmp as $t)
		{
			$id=$t['projekt_id'];
			while($id)
			{
				$ad=substr($GLOBALS['datetime'],0,6);
				$cd=substr($t['ist_beginn'],0,6);
				while(isset($prj[$id]['hist'][$cd]))
				{
					$prj[$id]['hist'][$cd]['i'] += $t['zeitaufwand'];
					if($pla[$t['zeit_id']])
						$prj[$id]['hist'][$cd]['p'] += $t['zeitaufwand'];
					if($t['rechnungs_id'])
						$prj[$id]['hist'][$cd]['r'] += $t['zeitaufwand'];
					if($cd < $ad)
					{
						$y=(int)substr($cd, 0, 4);
						$m=(int)substr($cd, 4, 2) + 1;
						if($m > 12)
						{
							$y++;
							$m=1;
						}
						$cd=substr('0000'.$y, -4).substr('00'.$m, -2);
					}
					else
						break;
				}

				$id=(int)$prj[$id]['elter'];
			}
		}
	}

//fxDebug($prj, 'Projektdaten inkl. Historie');

	// Maskenspalten
	if($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY)
	{
		$mn=489;
		$colspan=10;
	}
	else
	{
		$mn=488;
		$colspan=11;
	}
	$fa=fxf_getMaskFields($mn);
//fxDebug($fa,'$fa', 0);

	// Liste aufbereiten
	$z=0;
	$mc=$GLOBALS['_maskcounter'];
	$cnt=0;
	foreach($prj as $id => $p)
	{
		if($cnt && $_POST['Projekt_HistoryID'])
			fxf_maskColumnBorder($fa, 3, -1, $mc, $z, true);

		// Projekt
		$level=substr_count($p['vorgangsnummer'], '.');
		$txt  = str_repeat('&nbsp;', $level*5);
		$txt .= '<img src="'.$GLOBALS['gfxpath'].'pt'.$p['projektart'].'.png" align="top">';
		$txt .= '&nbsp;'.no_null($p['vorgangsnummer']);
		$ltxt=substr($p['name_projekt'], 0, 72 - $level*5);
		if(strlen($ltxt) < strlen($p['name_projekt']))
			$ltxt .= '...';
		$cls='darkgrey';
		if($p['projektart'] < FXP_PRJ_TASK)
			$cls .= ' bold';
		$txt .= '&nbsp;<font class="'.$cls.'" tooltip="'.$p['name_projekt'].'" style="cursor:help;">'.$ltxt.'</font>';
		$GLOBALS['fxptdata']['lvalues'][$mc][$z][$lfelder[0]]='~'.$txt;

		// Stat.
		$cls='black';
		if($p['projekt_status'] == FXP_PS_PLANNED)
			$cls='grey';
		else if($p['projekt_status'] == FXP_PS_ACTIVE)
			$cls .= ' bold';
		$txt='<font class="'.$cls.'">'.$status_text[$p['projekt_status']].'</font>';
		$GLOBALS['fxptdata']['lvalues'][$mc][$z][$lfelder[1]]='~'.$txt;

		// Start
		if($p['ist_beginn'])
			$txt=get_entry($p['ist_beginn'], 'datum');
		else
			$txt='-';
		$txt .= '<br><font class="grey">('.get_entry($p['soll_beginn'], 'datum').')</font>';
		$GLOBALS['fxptdata']['lvalues'][$mc][$z][$lfelder[2]]='~'.$txt;

		// Ende
		if($p['ist_ende'])
			$txt=get_entry($p['ist_ende'], 'datum');
		else
			$txt='-';
		$txt .= '<br><font class="grey">('.get_entry($p['soll_ende'], 'datum').')</font>';
		$GLOBALS['fxptdata']['lvalues'][$mc][$z][$lfelder[3]]='~'.$txt;

		// Tage
		if(!$p['ist_ende'])
		{
			if($p['deadline'])
			{
				$dtm=$p['deadline'];
				$cls='lightred';
			}
			else
			{
				$dtm=$p['soll_ende'];
				$cls='grey';
			}
			$diff=fxp_date_diff($dtm, $GLOBALS['datetime'], 'D');
			if($diff > 0)
				$txt='<font class="red">+&nbsp;'.abs($diff).'&nbsp;d</font>';
			else if($diff < 0)
				$txt='&nbsp;<font class="green">-&nbsp;'.abs($diff).'&nbsp;d</font>';
			else
				$txt='&nbsp;<font class="grey">=&nbsp;0&nbsp;d</font>';
			$txt .= '<br><font class="'.$cls.'">'.get_entry($dtm, 'datum').'</font>';
		}
		else
			$txt='';
		$GLOBALS['fxptdata']['lvalues'][$mc][$z][$lfelder[4]]='~'.$txt;

		// Hidden
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['id']=$id;
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_zahl5']=$p['aufwand_soll'];

		$cnt=0;
		if(fxIsArray($p['hist']))
		{
			foreach($p['hist'] as $cd => $ph)
			{
				$cls='';
				if($cnt)
				{
//					fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);

					$cls='^class_rv'.(($z+1)%2).' grey|';
					$txt='&nbsp;&middot;&nbsp;'.substr($cd, 0, 4).' <i>'.$monate_text[(int)substr($cd, 4, 2)].'</i>';
					$bt='';
					if($cnt == 1)
					{
						$bt='border-top:1px solid #'.$GLOBALS['msk_nbcol'].';';
						fxf_maskColumnBorder($fa, 3, -1, $mc, $z);
						fxf_maskColumnBorder(array('Projekt'), 2, -1, $mc, $z);
					}
					$GLOBALS['fxptdata']['lvalues'][$mc][$z][$lfelder[1]]=substr($cls,0,-1).'" nowrap colspan="4" style="'.$bt.'border-right:1px solid #'.$GLOBALS['msk_nbcol'].';"><!--|~'.$txt;
					$GLOBALS['fxptdata']['lvalues'][$mc][$z][$lfelder[4]]='~-->'.$txt;
				}

				// SOLL
				if(!$cnt && ($p['projektart'] == FXP_PRJ_TASK) && ($p['projekt_status'] != FXP_PS_COMPLETED))	// nicht abgeschl. Aufgabe
				{
					if($err)
						$GLOBALS['fxptdata']['lvalues'][$mc][$z][$lfelder[5]]=$_POST['nv_zahl1'][$mc][$z];
					else if($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY)
						$GLOBALS['fxptdata']['lvalues'][$mc][$z][$lfelder[5]]='~'.get_entry($p['aufwand_soll']/3600, "dezimal");
					else
						$GLOBALS['fxptdata']['lvalues'][$mc][$z][$lfelder[5]]=$p['aufwand_soll']/3600;
				}
				else if(!$cnt)
				{
					$tt='';
					if($ph['s'] != $p['aufwand_soll'])
					{
						$col='red';
						$tt=' tooltip="'.get_entry($ph['s']/3600, 'dezimal').'" style="cursor:help;"';
					}
					else
						$col='black';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z][$lfelder[5]]=$cls.'~<font class="'.$col.'"'.$tt.'>'.get_entry($p['aufwand_soll']/3600, 'dezimal').'</font>';
				}
				else
					$GLOBALS['fxptdata']['lvalues'][$mc][$z][$lfelder[5]]=$cls.'~'.get_entry($ph['s']/3600, "dezimal");
				$GLOBALS['fxptdata']['lvalues'][$mc][$z][$lfelder[6]]=$cls.'~h&nbsp;';

				// IST
				if(!$cnt)
				{
					$tt='';
					if($ph['i'] != $p['aufwand_ist'])
					{
						$col='red';
						$tt=' tooltip="'.get_entry($ph['i']/3600, 'dezimal').'" style="cursor:help;"';
					}
					else
						$col='black';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z][$lfelder[7]]=$cls.'~<font class="'.$col.'"'.$tt.'>'.get_entry($p['aufwand_ist']/3600, 'dezimal').'</font>';
				}
				else
					$GLOBALS['fxptdata']['lvalues'][$mc][$z][$lfelder[7]]=$cls.'~'.get_entry($ph['i']/3600, 'dezimal');
				$GLOBALS['fxptdata']['lvalues'][$mc][$z][$lfelder[7]] .= '&nbsp;h';

				// Prod.
				$GLOBALS['fxptdata']['lvalues'][$mc][$z][$lfelder[8]]=$cls.'~'.get_entry($ph['p']/3600, 'dezimal').'&nbsp;h';

				// Abger.
				$GLOBALS['fxptdata']['lvalues'][$mc][$z][$lfelder[9]]=$cls.'~'.get_entry($ph['r']/3600, 'dezimal').'&nbsp;h';

				// S/I (Fertigstellungsgrad)
				if($p['projekt_status'] == FXP_PS_COMPLETED)
					$fg=100.00;
				else if(!$cnt)
				{
					if($p['aufwand_soll'] != 0)
						$fg=$p['aufwand_ist']/$p['aufwand_soll'];
					else
						$fg=100.00;
				}
				else
				{
					if($ph['s'] != 0)
						$fg=$ph['i']/$ph['s'];
					else
						$fg=100.00;
				}
				$fg=$fg*100;
				if($fg == 0.00)
					$fcol='grey';
				else if($fg < 0.00)
				{
					$fg=0.00;
					$fcol='red';
				}
				else if($fg < 100.00)
					$fcol='black';
				else
				{
					$fg=100.00;
					$fcol='green';
				}
				if(!$cnt)
					$GLOBALS['fxptdata']['lvalues'][$mc][$z][$lfelder[10]]=$cls.'~<font class="'.$fcol.'">'.get_entry($fg, 'dezimal').'</font>&nbsp;%';
				else
					$GLOBALS['fxptdata']['lvalues'][$mc][$z][$lfelder[10]]=$cls.'~'.get_entry($fg, 'dezimal').'&nbsp;%';

				$cnt++;
				$z++;

				if((int)$cd <= (int)$ehm[$id])
					break;
				if(!$_POST['Projekt_HistoryID'] || (($_POST['Projekt_HistoryID'] != -1) && ($cnt > $_POST['Projekt_HistoryID'])))
					break;
				if(($p['projektart'] != FXP_PRJ_TASK) && !$_POST['ausfuehrlich'])
					break;
			}
		}
	}

	if($z)
	{
		fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
		$z++;
	}

	// Diagramm
	if($_POST['bild'] && isset($GLOBALS['_ext_chartdir']) && strlen($GLOBALS['_ext_chartdir']))
	{
		$hist=array();
		$cnt=0;
		foreach($prj[$pid]['hist'] as $monat => $d)
		{
			$hist[$monat]=$d;
			$cnt++;
			if($cnt > 24)
				break;
		}
		ksort($hist);

		$data=array();
		$labels=array();
		$maxsize_labels=0;

		$cnt=0;
		foreach($hist as $monat => $d)
		{
			$data['i'][$cnt]=$d['i']/3600.00;
			$data['p'][$cnt]=$d['p']/3600.00;
			$data['r'][$cnt]=$d['r']/3600.00;
			$data['s'][$cnt]=$d['s']/3600.00;

			$labels[$cnt]=$monate_text[(int)substr($monat, 4, 2)].' '.substr($monat, 0, 4);

			if(strlen($labels[$cnt]) > $maxsize_labels)
				$maxsize_labels=strlen($labels[$cnt]);

			$cnt++;
		}

		$cd_xoffset=65;
		$cd_yoffset=75;
		$cd_width=940;
		$fontsize=8;
		$cd_height=$cd_yoffset+256+$maxsize_labels*$fontsize;

		// Grafikbereich vorbereiten
		$c=new XYChart($cd_width, $cd_height);
		$c->setBackground($c->linearGradientColor(0, 0, 0, 100, 0xe0e0e0, 0xffffff), 0xbbbbbb);
		$c->setRoundedFrame(0xfeffffff);
		$c->setDropShadow(0xfeaaaaaa);

		// Diagrammbereich festlegen
		$c->setPlotArea($cd_xoffset, $cd_yoffset, $cd_width-$cd_xoffset-24, $cd_height-$cd_yoffset-$maxsize_labels*$fontsize+16, 0xfAfAfA, 0xfCfCfC, 0x888888, $c->dashLineColor(0xaaaabb, 0x000101), Transparent);

		// Titel
		$titleObj=$c->addTitle('<*block,valign=absmiddle*>'.no_null($prj[$pid]['vorgangsnummer']).' - '.$prj[$pid]['name_projekt'].'<*/*>', "arialbi.ttf", 12, 0x000000);
		$titleObj->setBackground(0xdedede, 0x000000, glassEffect());

		// Legende
		$legendObj=$c->addLegend($cd_xoffset, 25, false, '', $fontsize);
		$legendObj->setBackground(Transparent);

		// Daten
		$c->addAreaLayer($data['i'], 0x80009F6B, get_text(914, '20'), 3);	// IST-Stunden
		$c->addAreaLayer($data['p'], 0x80DDCC88, get_text(1612, '15'), 3);	// Produktiv
		$c->addAreaLayer($data['r'], 0x80006B9F, get_text(1950, '15'), 3);	// Abgerechnet
		$c->addAreaLayer($data['s'], 0x80DDDDDD, get_text(917, '20'), 3);	// SOLL-Stunden

		// Beschriftung Y-Achse
		$c->yAxis->setLabelStyle("arial.ttf", $fontsize, 0x444444);
		$c->yAxis->setTitle(meldung(1174).' (h)', "arialbd.ttf", $fontsize, 0x000000);	// Stunden

		// Beschriftung X-Achse
		$labelStyleObj=$c->xAxis->setLabelStyle("arial.ttf", $fontsize, 0x000000);
		$labelStyleObj->setFontAngle(60);
		$c->xAxis->setLabels($labels);

		// und als PNG zwischenspeichern
		$img=$c->makeChart2(PNG);
		$fname=saveChartDirectorImage($img, $GLOBALS['tmppath'].'~'.$GLOBALS['datetime'].$GLOBALS['fxpglobals']['person'].'g.png');
		if(strlen($fname))
		{
			// Diagramm anzeigen
			$txt='<br /><img src="'.$fname.'" border="0" width="'.$cd_width.'" height="'.$cd_height.'" title="">';
			$GLOBALS['fxptdata']['lvalues'][$mc][$z][$lfelder[0]]='^class_transparent" colspan="'.$colspan.'" align="center"><!--|~'.$txt;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z][$lfelder[10]]='~-->'.$txt;
		}
	}
}
?>