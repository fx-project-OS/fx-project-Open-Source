<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : stunden_info_aufgaben.inc                                    //
// Version     : 24.1                                                         //
// Begin       : 2021-03-30                                                   //
// Last Change : 2025-03-13                                                   //
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
 * PF 81: Time Recording (Day) - AJAX
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */


// Search for and include "basics.inc" to set all definitions, variables and necessary dynamic paths
$__pnm='basics.inc';
if(!isset($GLOBALS['__loaded_'.$__pnm]))
{
	$__prg=$__pnm;
	$__pfc=0;
	while(($__pfc < 10) && !file_exists($__prg))
	{
		$__pfc++;
		$__prg='../'.$__prg;
	}
	if(file_exists($__prg))
	{
		require($__prg);
	}
	else
		die('<div style="padding:8px;"><h1 style="padding:8px;color:red;background:#fff;border:2px solid red;box-shadow:4px 4px 4px rgba(0,0,0, 0.5);">fx-project - ERROR: Necessary file &quot;'.$__pnm.'&quot; not found!</h1></div>');
}

$GLOBALS['_form']='';
fxSession('t','l');

$pid=(int)$_GET['vb_pid'];
if(!$pid)
	$pid=(int)$vb_pid;
if(!$pid)
	$pid=0;

$mid=(int)$_GET['mid'];
if(!$mid)
	$mid=$GLOBALS['fxpglobals']['client'];

// Prüfen, ob Recht auf Person
$pgp=false;
if($mid != $GLOBALS['fxpglobals']['client'])
	$all_pers=false;
if($GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2])	// Verwaltung oder Geschäfstführer
{
	$sql="SELECT personen_id FROM personen WHERE mandanten_id=".$GLOBALS['mid'];
	$all_pers=db_values($sql, '*');
	$pgp=true;
}
else if($GLOBALS['fxpglobals']['persdat']['roles'][1])										// Projektleiter
{
	$sql="SELECT personen_id FROM personen WHERE mandanten_id=".$GLOBALS['mid']." AND (elter=".$GLOBALS['fxpglobals']['person']." OR personen_id=".$GLOBALS['fxpglobals']['person'].")";
	$all_pers=db_values($sql, '*');
}
else																			// Mitarbeiter
	$all_pers[0]['personen_id']=$GLOBALS['fxpglobals']['person'];
if($GLOBALS['fxpglobals']['person'] == $pid)
	$pgp=true;

if(fxIsArray($all_pers))
{
	$pos_pers ='';
	foreach($all_pers as $p)
		$pos_pers .= ','.$p['personen_id'];
	$pos_pers=substr((string)$pos_pers, 1);
}
else
	$pos_pers='-1';

$eps=explode(',',$pos_pers);
$pid_ok=in_array($pid,$eps);

$title=meldung(1565);	// Aufwandsübersicht aller ausgewählten Aufgaben
$pmwidth=640;
$pmheight=128;
$pmprint=false;

if($pid_ok)
{
	// Projekte einlesen
	$sarray=false;
	if(strlen((string)$GLOBALS['fxptdata']['backup']['aufids']))
	{
		$sql  = "SELECT";
		$sql .= " p.projekt_id, p.name_projekt, p.vorgangsnummer, p.projektart, p.aufwand_ist, p.aufwand_soll, p.max_zeit_aufw, p.deadline, p.projekt_status, p.prioritaet,";
		$sql .= " m.soll_beginn, m.soll_ende, m.auslastung, m.aufwand_std, m.prozent ";
		$sql .= "FROM";
		$sql .= " projekte p, maplanung m ";
		$sql .= "WHERE";
		$sql .= " p.projekt_id=m.projekt_id AND m.personen_id=".$pid." AND p.projekt_id IN (".$GLOBALS['fxptdata']['backup']['aufids'].")";
		$sql .= " AND p.mandanten_id=".$mid." AND m.mandanten_id=".$mid." ";
		$sql .= "ORDER BY p.";
		// nach Projektnummern sortieren
		if(strtolower(substr((string)$GLOBALS['fxpglobals']['settings']['zeit_projektnr'], 0, 1)) == 'j')
			$sql .= "vorgangsnummer";
		else
			$sql .= "name_projekt";
		$sarray=db_values($sql, '*');
//fxDebug($sarray, $sql);
	}

	if(fxIsArray($sarray))
	{
		$pmwidth=-1;
		$pmheight=-1;
		$pmprint=true;

		// Alle Methoden ermitteln
		$tmethod=fxf_getReferenceValues('methode');
//fxDebug($tmethod, $sql);

		$inhalt  = '<table width="100%" border="0" cellspacing="1" cellpadding="2">'.$nl;
		$inhalt .= '	<tr>'.$nl;
		// Projektname
		$inhalt .= '		<td class="rhd" width="50%" nowrap align="center">'.get_text(521, '20').'</td>'.$nl;
		// Methode
		$inhalt .= '		<td class="rhd" nowrap align="center">'.substr((string)get_text(1659,'10'),0,4).'.</td>'.$nl;
		// Soll-Zeitraum
		$inhalt .= '		<td class="rhd" nowrap align="center">'.meldung(1114, false).'</td>'.$nl;
		// Delta-Zeitraum
		$inhalt .= '		<td class="rhd" nowrap align="center">&Delta;-'.meldung(871, false).'</td>'.$nl;
		// Deadline
		if($GLOBALS['fxpglobals']['persdat']['role'] > 0)
			$inhalt .= '	<td class="rhd" nowrap align="center">'.get_text(48, '20').'</td>'.$nl;
		// Ressources
		$lit_ressources=get_text(97,'5');
		if($GLOBALS['fxpglobals']['persdat']['role'] > 0)
			$inhalt .= '	<td class="rhd" nowrap align="center">'.$lit_ressources.'</td>'.$nl;
		// Prozent
		$lit_prozent='%';
		$inhalt .= '		<td class="rhd" nowrap align="center">'.$lit_prozent.'</td>'.$nl;
		// Aufwand-Soll
		$lit_aufwand_soll=get_text(297, '10');
		$inhalt .= '		<td class="rhd" nowrap align="center">'.$lit_aufwand_soll.'</td>'.$nl;
		// Aufwand-Ist
		$lit_aufwand_ist=get_text(298, '10');
		$inhalt .= '		<td class="rhd" nowrap align="center">'.$lit_aufwand_ist.'</td>'.$nl;
		// Delta-Aufwand
		$lit_delta_aufwand='&Delta;-'.meldung(436,false);
		$inhalt .= '		<td class="rhd" nowrap align="center">'.$lit_delta_aufwand.'</td>'.$nl;
		// Max. Zeitaufwand
		if($GLOBALS['fxpglobals']['persdat']['role'] > 1)
			$inhalt .= '	<td class="rhd" nowrap align="center" colspan="2">'.get_text(132, '10').'</td>'.$nl;
		// Balkendiagramm
		$inhalt .= '		<td class="rhd" width="50%" nowrap align="center">'.meldung(1566).'</td>'.$nl;
		$inhalt .= '	</tr>'.$nl;

		$sn=0;
		$jetzt=fxNow();
		foreach($sarray as $sa)
		{
			// Ressourcen
			$ra=array();
			$sql  = "SELECT m.personen_id, m.soll_beginn, m.soll_ende, m.auslastung, m.aufwand_std, 0 AS aufwand_ist, m.prozent, p.pname, p.vorname FROM maplanung m, personen p";
			$sql .= " WHERE m.projekt_id=".$sa['projekt_id']." AND m.mandanten_id=".$GLOBALS['mid']." AND p.personen_id=m.personen_id AND p.mandanten_id=".$GLOBALS['mid'];
			$sql .= " ORDER BY p.pname, p.vorname";
			$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
			if(fxIsArray($tmp))
			{
				foreach($tmp as $t)
					$ra[(int)$t['personen_id']]=$t;
			}

			// Soll-Aufwand
			$szd=0.0;
			if(isset($ra[(int)$GLOBALS['pid']]))
				$szd=get_double($ra[(int)$GLOBALS['pid']]['aufwand_std']);

			// Aufwand
			$izd=0.0;
			$sql  = "SELECT personen_id, SUM(zeitaufwand) AS aufwand_ist FROM zeitdaten";
			$sql .= " WHERE projekt_id=".$sa['projekt_id']." AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND mandanten_id=".$GLOBALS['mid'];
			$sql .= " GROUP BY personen_id";
			$aa=db_values($sql,'*');
//fxDebug($aa,$sql);
			if(fxIsArray($aa))
			{
				foreach($aa as $ac => $a)
				{
					$ap=(int)$a['personen_id'];
					if(!isset($ra[$ap]))
					{
						$ra[$ap]=array('personen_id'=>$ap, 'soll_beginn'=>"", 'soll_ende'=>"", 'auslastung'=>0.0, 'aufwand_std'=>0, 'aufwand_ist'=>0, 'prozent'=>0.0, 'pname'=>"?", 'vorname'=>"");
						$sql="SELECT pname, vorname FROM personen WHERE personen_id=".$ap." AND mandanten_id=".$GLOBALS['mid'];
						$tmp=db_values($sql);
						if(fxIsArray($tmp))
						{
							$ra[$ap]['pname']=$tmp['pname'];
							$ra[$ap]['vorname']=$tmp['vorname'];
						}
					}

					$ra[$ap]['aufwand_ist']=(int)$a['aufwand_ist'];

					if($ap == (int)$GLOBALS['pid'])
						$izd=get_double($ra[$ap]['aufwand_ist']);
				}
			}
//fxDebug($ra,'$ra');

			// Projektfortschritt
			$sql="SELECT methode, zeitpunkt, tage FROM projektfortschritt WHERE projekt_id=".$sa['projekt_id']." AND (personen_id IS NULL OR personen_id=0) AND aktiv_kz=1 AND mandanten_id=".$GLOBALS['mid'];
			$pfm=db_values($sql);
			if(!is_array($pfm))
				$pfm=array('methode'=>1784, 'zeitpunkt'=>1790, 'tage'=>0);

			$cls=' class="rv'.($sn%2).'"';
			$clsn=$cls.' nowrap';
			$clsv=$cls.' valign="top"';
			$clsnv=$clsn.' valign="top"';

			$fcls='s3';

			$inhalt .= '<tr>'.$nl;

			// Projektname
			$pn=$sa['name_projekt'];
			$pt='';
			if(strlen((string)$pn) > 32)
			{
				$pn=text_wrap($sa['name_projekt'],false,32).'<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top">';
				$pt=' tooltip="'.str_replace('"', '&quot;', $sa['name_projekt']).'" style="cursor:help;"';
			}
			$inhalt .= '<td'.$clsnv.'><font class="'.$fcls.' grey">'.no_null($sa['vorgangsnummer']).'</font> <font class="'.$fcls.'"'.$pt.'>'.$pn.'</font></td>';

			// Methode
			$mhn='Methode'.$pfm['methode'];
			$metl=$GLOBALS['tmethod'][$pfm['methode']];
			$mpp=strpos($metl,'(');
			if($mpp)
				$metl=trim(substr((string)$metl,0,$mpp));
			$inhalt .= '<td'.$clsnv.' align="center" tooltip="'.$metl.'" style="cursor:help;">';
			if($pfm['methode'] == 1784)
				$inhalt .= '<i class="'.$fcls.' grey">'.substr((string)$metl,0,3).'&hellip;</i>';
			else
				$inhalt .= '<span class="'.$fcls.'">'.substr((string)$metl,0,3).'&hellip;</span>';
			$inhalt .= '<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></td>'.$nl;

			// Zeitraum
			$inhalt .= '<td'.$clsnv.' align="center">';
			if($sa['soll_beginn'] < $jetzt)
				$ccls='red';
			else
				$ccls='black';
			$inhalt .= '<font class="'.$fcls.' '.$ccls.'">'.get_entry($sa['soll_beginn'], 'datum').'</font><font class="grey">-</font>';
			if($sa['soll_ende'] < $jetzt)
				$ccsl='red';
			else
				$ccls='black';
			$inhalt .= '<font class="'.$fcls.' '.$ccls.'">'.get_entry($sa['soll_ende'], 'datum').'</font></td>'.$nl;

			// Delta-Termin
			$tagdiff=fxp_date_diff($sa['soll_ende'], $jetzt);
			if($tagdiff > 0)
			{
				$tagdiff='+'.$tagdiff;
				$ccls='red';
			}
			else if($tagdiff < 0)
				$ccls='green';
			else
				$ccls='grey';
			$inhalt .= '<td'.$clsnv.' align="right"><font class="'.$fcls.' '.$ccls.'">'.$tagdiff.'</font>&nbsp;<font class="'.$fcls.' grey">d</font></td>'.$nl;

			// Deadline
			if($GLOBALS['fxpglobals']['persdat']['role'] > 0)
			{
				if($sa['deadline'])
				{
					if($sa['deadline'] < $jetzt)
						$ccls='red';
					else
						$ccls='green';
					$inhalt .= '<td'.$clsnv.' align="center"><font class="'.$fcls.' '.$ccls.'">'.get_entry($sa['deadline'], 'datum').'</font></td>'.$nl;
				}
				else
					$inhalt .= '<td'.$clsnv.' align="center"><font class="'.$fcls.' grey">-</font></td>'.$nl;
			}

			// Ressourcen
			if($GLOBALS['fxpglobals']['persdat']['role'] > 0)
			{
				$rt='';
				if(fxIsArray($ra))
				{
					$rt  = ' tooltip="<table>';
					$rt .= '	<tr>';
					$rt .= '		<td class=rhd nowrap align=center>'.$GLOBALS['lit_ressources'].'</td>';
					$rt .= '		<td class=rhd nowrap align=center>'.$GLOBALS['lit_prozent'].'</td>';
					$rt .= '		<td class=rhd nowrap align=center>'.$GLOBALS['lit_aufwand_soll'].'</td>';
					$rt .= '		<td class=rhd nowrap align=center>'.$GLOBALS['lit_aufwand_ist'].'</td>';
					$rt .= '		<td class=rhd nowrap align=center>'.$GLOBALS['lit_delta_aufwand'].'</td>';
					$rt .= '	</tr>';
					$sas=0;
					$sai=0;
					$rc=0;
					foreach($ra as $rid => $r)
					{
						$sas += $r['aufwand_std'];
						$sai += $r['aufwand_ist'];

						if($rid == (int)$GLOBALS['pid'])
							$tdc=' style=\'background:#eeffee;\'';
						else if(!$r['aufwand_std'])
							$tdc=' style=\'background:#ffeeee;\'';
						else if($rc%2)
							$tdc=' style=\'background:#f0f0f0;\'';
						else
							$tdc=' style=\'background:#f8f8f8;\'';

						$rt .= '	<tr>';
						// ...Ress.
						$rn='<b>'.$r['pname'].'</b>';
						if(strlen((string)$r['vorname']))
							$rn .= ', '.$r['vorname'];
						$rt .= '		<td nowrap'.$tdc.'>'.$rn.'</td>';
						// ...%
						$rt .= '		<td nowrap align=right'.$tdc.'><font class=\''.$fcls.' grey\'>'.get_entry($r['prozent']*100,'ganzzahl').'%</font></td>';
						// ...Aufw. Soll
						if($r['aufwand_std'] == 0)
							$ccls='lightgrey';
						else
							$ccls='black';
						$rt .= '<td nowrap align=right'.$tdc.'><font class=\''.$fcls.' '.$ccls.'\'>'.get_entry($r['aufwand_std'],'zeitspanne').'</font>&nbsp;<font class=\''.$fcls.' grey\'>h</font></td>';
						// ...Aufw. Ist
						if($r['aufwand_ist'] == 0)
							$ccls='lightgrey';
						else
							$ccls='black';
						$rt .= '<td nowrap align=right'.$tdc.'><font class=\''.$fcls.' '.$ccls.'\'>'.get_entry($r['aufwand_ist'], 'zeitspanne').'</font>&nbsp;<font class=\''.$fcls.' grey\'>h</font></td>';
						// ...Delta-Aufwand
						if($r['aufwand_std'] != 0.00)
						{
							$diff=$r['aufwand_ist']-$r['aufwand_std'];
							if($diff < 0)
								$ccls='green';
							else if($diff == 0)
								$ccls='grey';
							else
								$ccls='red';
							$diff_str='<font class=\''.$fcls.' '.$ccls.'\'>'.get_entry($diff, 'zeitspanne').'</font>';
							$p_as=1;
						}
						else
						{
							$diff=0;
							$diff_str='<font class=\''.$fcls.' lightgrey\'>'.get_entry($szd, 'zeitspanne').'</font>';
							$p_as=0;
						}
						$rt .= '<td nowrap align=right'.$tdc.'>'.$diff_str.'&nbsp;<font class=\''.$fcls.' grey\'>h</font></td>';
						$rt .= '	</tr>';

						$rc++;
					}

					// Summe
					$tdc=' style=\'background:#eeeeff;\'';
					$rt .= '	<tr><td colspan=5><hr /></td></tr>';
					$rt .= '	<tr>';
					$rt .= '		<td colspan=2'.$tdc.'>&sum;</td>';
					// ...Aufw. Soll
					if($sas == 0)
						$ccls='lightgrey';
					else
						$ccls='black';
					$rt .= '		<td nowrap align=right'.$tdc.'><b class=\''.$fcls.' '.$ccls.'\'>'.get_entry($sas,'zeitspanne').'</b>&nbsp;<font class=\''.$fcls.' grey\'>h</font></td>';
					// ...Aufw. Ist
					if($sai == 0)
						$ccls='lightgrey';
					else
						$ccls='black';
					$rt .= '		<td nowrap align=right'.$tdc.'><b class=\''.$fcls.' '.$ccls.'\'>'.get_entry($sai, 'zeitspanne').'</b>&nbsp;<font class=\''.$fcls.' grey\'>h</font></td>';
					// ...Delta-Aufwand
					if($sas != 0.00)
					{
						$diff=$sai-$sas;
						if($diff < 0)
							$ccls='green';
						else if($diff == 0)
							$ccls='grey';
						else
							$ccls='red';
						$diff_str='<b class=\''.$fcls.' '.$ccls.'\'>'.get_entry($diff, 'zeitspanne').'</b>';
						$p_as=1;
					}
					else
					{
						$diff=0;
						$diff_str='<font class=\''.$fcls.' lightgrey\'>'.get_entry($szd, 'zeitspanne').'</font>';
						$p_as=0;
					}
					$rt .= '		<td nowrap align=right'.$tdc.'>'.$diff_str.'&nbsp;<font class=\''.$fcls.' grey\'>h</font></td>';
					$rt .= '	</tr>';
					$rt .= '</table>" style="cursor:help;"';
				}
				$inhalt .= '<td'.$clsnv.' align="right"'.$rt.'><font class="'.$fcls.' grey">'.sizeof($ra).'<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></font></td>'.$nl;
			}

			// Prozent
			$inhalt .= '<td'.$clsnv.' align="right"><font class="'.$fcls.' grey">'.get_entry($sa['prozent']*100, 'ganzzahl').'%</font></td>'.$nl;

			// Aufwand Soll
			if($szd == 0)
				$ccls='lightgrey';
			else
				$ccls='black';
			$inhalt .= '<td'.$clsnv.' align="right"><font class="'.$fcls.' '.$ccls.'">'.get_entry($szd, 'zeitspanne').'</font>&nbsp;<font class="'.$fcls.' grey">h</font></td>'.$nl;

			// Aufwand Ist
			if($izd == 0)
				$ccls='lightgrey';
			else
				$ccls='black';
			$inhalt .= '<td'.$clsnv.' align="right"><font class="'.$fcls.' '.$ccls.'">'.get_entry($izd, 'zeitspanne').'</font>&nbsp;<font class="'.$fcls.' grey">h</font></td>'.$nl;

			// Delta-Aufwand
			if($szd != 0.00)
			{
				$diff=$izd-$szd;
				if($diff < 0)
					$ccls='green';
				else if($diff == 0)
					$ccls='grey';
				else
					$ccls='red';
				$diff_str='<font class="'.$fcls.' '.$ccls.'">'.get_entry($diff, 'zeitspanne').'</font>';
				$p_as=1;
			}
			else
			{
				$diff=0;
				$diff_str='<font class="'.$fcls.' lightgrey">'.get_entry($szd, 'zeitspanne').'</font>';
				$p_as=0;
			}
			$inhalt .= '<td'.$clsnv.' align="right">'.$diff_str.'&nbsp;<font class="'.$fcls.' grey">h</font></td>'.$nl;

			// Max. Zeitaufwand
			if($GLOBALS['fxpglobals']['persdat']['role'] > 1)
				$inhalt .= '<td'.$clsnv.' align="right"><font class="'.$fcls.' grey">'.get_entry($sa['aufwand_ist'], 'zeitspanne').'</font>&nbsp;<font class="'.$fcls.' grey">h</font></td>'.$nl;
			if($sa['max_zeit_aufw'] && ((double)$sa['max_zeit_aufw'] != 0.00))
			{
				$p_maxza=get_entry($sa['max_zeit_aufw'], 'zeitspanne');
				$p_mza=1;
			}
			else
			{
				$p_maxza=get_entry(0.00, 'zeitspanne');
				$p_mza=0;
			}
			if($GLOBALS['fxpglobals']['persdat']['role'] > 1)
				$inhalt .= '<td'.$clsnv.' align="right"><font class="'.$fcls.'">'.$p_maxza.'</font>&nbsp;<font class="'.$fcls.' grey">h</font></td>'.$nl;

			// Balkendiagramm
			$height='8';
			$inhalt .= '<td'.$clsn.' width="100%">'.$nl;
			$inhalt .= '<table width="100%" border="0" cellpadding="0" cellspacing="0">'.$nl;
			$inhalt .= '<tr height="'.$height.'">'.$nl;
			if($diff > 0)
			{
				if($p_mza && $p_as && ($izd > $sa['max_zeit_aufw']))
					$inhalt .= '<td class="barplusm" height="'.$height.'" width="100%"></td>'.$nl;
				else
					$inhalt .= '<td class="barplus" height="'.$height.'" width="100%"></td>'.$nl;
			}
			else if($diff < 0)
			{
				$bg=round(($izd/$szd)*100);
				$br=100-$bg;
				if($bg > 0)
					$inhalt .= '<td class="barnegi" height="'.$height.'" width="'.$bg.'%"></td>'.$nl;
				if($br > 0)
					$inhalt .= '<td class="barnegs" height="'.$height.'" width="'.$br.'%"></td>'.$nl;
			}
			else
			{
				if($p_mza && ($izd > $sa['max_zeit_aufw']))
					$inhalt .= '<td class="barplusm" height="'.$height.'" width="100%"></td>'.$nl;
				else
					$inhalt .= '<td class="barnull" height="'.$height.'" width="100%"></td>'.$nl;
			}
			$inhalt .= '</tr>'.$nl;
			$inhalt .= '</table>'.$nl;
			$inhalt .= '</td>'.$nl;
			$inhalt .= '</tr>'.$nl;

			$sn++;
		}
		$inhalt .= '</table>'.$nl;
	}
	else
	{
		// Es wurden keine passenden Einträge gefunden.
		$inhalt='<table width=100% height=100%><tr><td width=100% align=center><h2 class="darkgrey">'.meldung(712,true).'</h2></td></tr></table>';
	}
}
else
{
	// Sie haben nicht die Berechtigung diese Transaktion aufzurufen.
	$inhalt='<table width=100% height=100%><tr><td width=100% align=center><h1 class="red">'.meldung(413,true).'</h1></td></tr></table>';
}

echo(popupMask($title, '', $inhalt, 'dtxsi', $pmwidth, $pmheight, $pmprint));
?>