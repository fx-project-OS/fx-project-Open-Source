<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : stunden_tag_liste_pre.inc                                    //
// Version     : 24.1                                                         //
// Begin       : 2020-11-09                                                   //
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
 * PF 81: Time Recording (Day)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

if(!isset($GLOBALS['app_wd']))
	$GLOBALS['app_wd']=0;

$tr_table=array(chr(0x5C).chr(0x22)=>'"', $nl.'<br />'.$nl=>$nl, '<br />'.$nl=>$nl, '<br>'=>$nl);
$mnr_gw=4;
$mnr_gj=$mnr_gw+1;

$icon_aendern_lit='*';

$mst=true;
if($sum && $Zeit_ID)
{
	$sql="SELECT zeit_id,zeiteinheit,freigabe,rechnungs_id,nicht_abrechenbar FROM zeitdaten WHERE (zeit_id=".$Zeit_ID." OR zeiteinheit=".$Zeit_ID.") AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY ist_beginn";
	$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
		{
			if(($t['zeiteinheit'] != $Zeit_ID) || ($t['rechnungs_id']) || ($t['freigabe'] && ($t['freigabe'] > ($GLOBALS['fxpglobals']['persdat']['role']+1))))
			{
				$mst=false;
				break;
			}
		}
		// Dieser Master-Zeitsummensatz kann nur noch beschränkt abgeändert werden, da bereits mindestens ein von diesem Mastersatz abhängiger Zeitsummensatz abgerechnet oder von einer höheren Instanz freigegeben wurde!
		if(!$mst && ($tmp[0]['zeiteinheit'] == $Zeit_ID) && ($Dummy == FXP_CHANGE))
			error_msg(3005,false);
	}
}
//fxDebug($mst,'$mst');

// Füllen von Zeiteinheit_Ist_Verz der selbe SQL-Befehl
if(!$sum)
{
	$Zeiteinheit_Ist_Verzselect		= "SELECT tabwert, wertetabid FROM wertetab WHERE id_feld=330 AND wertetabid<>856 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0 ORDER BY position";
	$Zeiteinheit_Ist_Verzselectfeld	= 'tabwert';
	$Zeiteinheit_Ist_Verzselectwert	= 'wertetabid';
}

$out='&nbsp;&middot;&nbsp;<b>'.meldung(1943,false).'</b>';	// Überstundenauszahlung
$ltg=meldung(1255,false);									// Tage
$ldt=get_text(273,'10');									// Datum
$lfr=get_text(1285,'10');									// Freigabe
$lab=get_text(1826,'text');									// Abgerechnet
$indfp='<b class=grey>'.ucfirst(meldung(3280)).'</b>';		// Bereits indirekt abgerechnet (Festpreis!)
$abger='<b class=black>'.ucfirst(meldung(1686)).'</b>';		// Bereits abgerechnet
$nverr='<b class=red>'.ucfirst(meldung(3275)).'</b>';		// Nicht verrechenbar
$lzt=get_text(331,'10');									// Zeit
$stt=explode('|',meldung(3002,false));						// Master|Teilsumme|Hier klicken um zum Master-Eintrag zu springen.|Reisekosten-Eintrag bearbeiten|Zeiterfassungs-Eintrag bearbeiten|Zeitsummen-Eintrag bearbeiten
$ita=explode('|',meldung(3003,false));						// Vorhandene Einträge bearbeiten|Neuen Eintrag hinzufügen|Zur Monatsansicht wechseln|Zur Wochenansicht wechseln|Zur Tagesansicht wechseln|Zur Materialverbrauchseingabe wechseln|Zur Zeiterfassung wechseln
//fxDebug($ita,'$ita');

// 1: MA, 2: PL, 3: VW/GF
$txt_rl=array(1=>array('txt'=>meldung(2278), 'col'=>'grey'), 2=>array('txt'=>meldung(2279), 'col'=>'blue'), 3=>array('txt'=>meldung(2280), 'col'=>'green'));

$ico_leer='<div style="float:left;margin:3px;margin-bottom:0;width:13px;height:13px;"></div>';
$ico_nverr='<img src="'.$GLOBALS['gfxpath'].'nnl_c_13x13.png" width="13" height="13" tooltip="'.$nverr.'" style="float:left;margin:3px;margin-bottom:0;cursor:help;">';
$ico_rchng='<img src="'.$GLOBALS['gfxpath'].'wrn_b_13x13.png" width="13" height="13" tooltip="'.$abger.' &rarr; <b>{RN}</b>" style="float:left;margin:3px;margin-bottom:0;cursor:help;">';
$ico_rchfp='<img src="'.$GLOBALS['gfxpath'].'wrn_b_13x13.png" width="13" height="13" tooltip="'.$indfp.' &rarr; <b>{RN}</b>" style="float:left;margin:3px;margin-bottom:0;opacity:0.5;cursor:help;">';
$ico_rl=array(
	1=>'<div tooltip="'.$lfr.': <b>'.$txt_rl[1]['txt'].'</b>" style="position:relative;float:left;margin:2px;width:13px;height:13px;border:1px solid #888888;border-right-color:#000000;border-bottom-color:#000000;border-radius:3px;cursor:help;"><div class="'.$txt_rl[1]['col'].'" style="position:absolute;top:1px;font-size:8px;">'.substr((string)$txt_rl[1]['txt'],-2).'</div></div>',
	2=>'<div tooltip="'.$lfr.': <b>'.$txt_rl[2]['txt'].'</b>" style="position:relative;float:left;margin:2px;width:13px;height:13px;border:1px solid #888888;border-right-color:#000000;border-bottom-color:#000000;border-radius:3px;cursor:help;"><div class="'.$txt_rl[2]['col'].'" style="position:absolute;top:1px;left:1px;font-size:8px;">'.substr((string)$txt_rl[2]['txt'],-2).'</div></div>',
	3=>'<div tooltip="'.$lfr.': <b>'.$txt_rl[3]['txt'].'</b>" style="position:relative;float:left;margin:2px;width:13px;height:13px;border:1px solid #888888;border-right-color:#000000;border-bottom-color:#000000;border-radius:3px;cursor:help;"><div class="'.$txt_rl[3]['col'].'" style="position:absolute;top:1px;left:1px;font-size:8px;">'.substr((string)$txt_rl[3]['txt'],-2).'</div></div>'
);

$rightmu=tr_rechte(102,-$vb_pid);	// Recht für Materialverbrauchs-Programmfunktion
$icon_aendern_nw=true;

$d_delta=false;

$cb_fields=array(
	'Arbeitstage'=>"Arbeitstage", 'Urlaub'=>"Urlaub", 'sonderurlaub'=>"sonderurlaub", 'Schulung'=>"Schulungstage", 'Krank'=>"Krankheitstage", 'abwesenheit'=>"abwesenheit", 'Gleitzeit'=>"GleitzeitStunden",
	'leer_anzeigen'=>"leer_anzeigen", 'stunden'=>"SOLLStunden"
);
foreach($cb_fields as $cb_field => $dummy)
	${'cb_'.strtolower((string)$cb_field)}=true;

$maskenfeldwerte=array();
$mc=$GLOBALS['_maskcounter'];

// Tagesbericht
//fxDebug($gd,'$gd');
if($gd)
{
	// Leistungart ausblenden, falls nicht aktiviert
	if(!$kr_aktiv)
		$leistungsarten_nv=true;
	else
		$leistungsarten_nw=true;

	// Keine Trennlinien bei:
	$icon_anzeigen_nbd=true;
	$icon_aendern_nbd=true;
	$icon_loeschen_nbd=true;

	if($GLOBALS['app_wd'])
		$icon_anzeigen_nv=true;

	// Tagesübersichtsmaske
	$von=substr((string)$datum,0,8).'000000';
	$bis=substr((string)$datum,0,8).'240000';

	$mlistensql  = "SELECT z.ist_beginn AS ist_beg_zeit, z.ist_ende AS ist_end_zeit, z.pause, z.zeitaufwand, z.zeit_id, z.rechnungs_id, z.zeitart, z.zeiteinheit, z.freigabe, z.nicht_abrechenbar,";
	$mlistensql .= " z.rk_art, z.rk_kategorie, z.rk_ort1, z.rk_ort2, z.rk_km,".$nl;
	$mlistensql .= " t.taetigkeit_text, t.taetigkeit_id,".$nl;
	$mlistensql .= " p.name_projekt, p.vorgangsnummer, p.projekt_id, p.aufwand_soll, p.aufwand_ist, p.max_zeit_aufw, p.deadline, p.projektart, p.projekt_status ".$nl;

	$mlistensql .= "FROM zeitdaten z,taetigkeiten t,projekte p ".$nl;

	$mlistensql .= "WHERE z.zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.") AND z.ist_beginn<='".$bis."' AND z.ist_ende>='".$von."'";
	$mlistensql .= " AND t.zeit_id=z.zeit_id AND t.personen_id=".$vb_pid." AND p.projekt_id=z.projekt_id ";
	$mlistensql .= " AND z.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND t.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].") ".$nl;

	$mlistensql .= "ORDER BY z.ist_beginn,";
	if(strtolower((string)$GLOBALS['fxpglobals']['settings']['zeit_projektnr']) == 'ja')
		$mlistensql .= "p.vorgangsnummer";
	else
		$mlistensql .= "p.name_projekt";

	// Listenwerte füllen
	$lw=db_values($mlistensql, '*');
	$i=0;
	if(fxIsArray($lw))
	{
		// Nowrap
		$Zeitdaten_nw=true;
		$Pause_nw=true;
		$Name_Projekt_nw=true;
		$leistungsarten_nw=true;
		$Status_Benutzer_nw=true;
		$Zeitaufwand_nw=true;

		// Letzte Endzeit
		$letime='';

		// Rechnungsnummer Array
		$rnarr=array();

		$GLOBALS['fxptdata']['lvalues'][$mc]=array();
		foreach($lw as $zeile)
		{
			if(($zeile['zeitart'] == FXP_TE_TRAVELEXP) && $zeile['zeiteinheit'])
				continue;

			// Zeiten
			$stime='';
			$etime='';
			if((($zeile['zeitart'] == FXP_TE_TIMEREC) || ($zeile['zeitart'] == FXP_TE_TRAVELEXP_TIMEREC)) && ($zeile['ist_beg_zeit'] != $zeile['ist_end_zeit']))
			{
				$stime=get_entry($zeile['ist_beg_zeit'], 'zeit');
				$etime=get_entry($zeile['ist_end_zeit'], 'zeit');
			}

			// Pausenzeile?
			if(strlen((string)$letime) && strlen((string)$stime) && ($letime != $stime))
			{
				$sts=timeToSec($letime);
				$ets=timeToSec($stime);

				$bmsg='<i class="s2 grey">('.meldung(3134,false).')</i>';	// Keine Zeiterfassung vorhanden!

				$csp=5;
				if(isset($leistungsarten_nv) && $leistungsarten_nv)
					$csp--;

				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeitdaten']		= '~<font class="lightgrey">'.$letime.'-'.$stime.'</font>';
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['pause']			= '~<font class="lightgrey">'.get_entry($ets-$sts, 'zeitspanne').'</font>';;
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['name_projekt']	= '^class_?" nowrap align="center" colspan="'.$csp.'" style="border-right:1px solid #c4c4c4;box-shadow:2px 2px 4px rgba(0,0,0,0.2) inset;"><!--|~'.$bmsg;
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeitaufwand']		= '~-->'.$bmsg;

				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['icon_anzeigen']	= '~'.fieldLinkImage('new', 'icne', '?aktion=3&gd='.substr((string)$datum,0,8).substr((string)str_replace(':','',$letime).'000000',0,6).'&et='.substr((string)str_replace(':','',$stime).'000000',0,6).'&persid='.$vb_pid, meldung(1917,false), 81);	// Neue Zeiterfassung
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['icon_aendern']	= '~';
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['icon_loeschen']	= '~';

				if($GLOBALS['app_wd'])
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['icon_aendern']='~<a nid="btnnew" btn="action" data-role="button" data-icon="plus" data-theme="b" style="margin:0 0 6px 16px;">'.$app_lit_new.'</a>';

				$i++;
			}

			if(strlen((string)$etime))
				$letime=$etime;

			$GLOBALS['fxptdata']['lvalues'][$mc][$i]=$zeile;

			if(!strlen((string)$GLOBALS['fxptdata']['lvalues'][$mc][$i]['projekt_status']))	// Buchung auf "Unbekannt"
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['projekt_status']=FXP_PS_ACTIVE;

			// Tätigkeit
			$GLOBALS['fxptdata']['lvalues'][$mc][$i]['taetigkeit_text']=$GLOBALS['fxptdata']['lvalues'][$mc][$i]['taetigkeit_text'];

			// Icon "Anzeigen"
			$GLOBALS['fxptdata']['lvalues'][$mc][$i]['icon_anzeigen']='~'.fieldLinkImage('icon_anzeigen', 'icsh', 'loader.php?url=stunden_info_detail.inc&__tr=81&zid='.$GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeit_id'].'&mid='.$GLOBALS['fxpglobals']['client'].'&pid='.$vb_pid, $i_anzeigen);

			$ttcls='black';
			// Reise
			if(($GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeitart'] == FXP_TE_TRAVELEXP) || ($GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeitart'] == FXP_TE_TRAVELEXP_TIMEREC))
			{
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['pause']='~&nbsp;';

				if($GLOBALS['fxptdata']['lvalues'][$mc][$i]['ist_beg_zeit'] < $von)
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['ist_beg_zeit']=$von;
				if($GLOBALS['fxptdata']['lvalues'][$mc][$i]['ist_end_zeit'] > $bis)
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['ist_end_zeit']=$bis;
				if($GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeitart'] == FXP_TE_TRAVELEXP_TIMEREC) // Zeitkonto-Reise
				{
					$zd_ibz=$GLOBALS['fxptdata']['lvalues'][$mc][$i]['ist_beg_zeit'];
					$zd_iez=$GLOBALS['fxptdata']['lvalues'][$mc][$i]['ist_end_zeit'];
					$zab=fx_mktime(substr((string)$zd_ibz, 8, 2), substr((string)$zd_ibz, 10, 2), substr((string)$zd_ibz, 12, 2), substr((string)$zd_ibz, 4, 2), substr((string)$zd_ibz, 6, 2), substr((string)$zd_ibz, 0, 4));
					$zae=fx_mktime(substr((string)$zd_iez, 8, 2), substr((string)$zd_iez, 10, 2), substr((string)$zd_iez, 12, 2), substr((string)$zd_iez, 4, 2), substr((string)$zd_iez, 6, 2), substr((string)$zd_iez, 0, 4));
					$lw_zaw=$zae - $zab;
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeitaufwand']=$lw_zaw;
				}
				else
				{
					$ttcls='grey';
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeitaufwand']='~&nbsp;';
				}

				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['taetigkeit_text']  = '~<font class="'.$ttcls.'">'.meldung($GLOBALS['fxptdata']['lvalues'][$mc][$i]['rk_art']).'</font><br>';
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['taetigkeit_text'] .= '<font class="'.$ttcls.'">'.$GLOBALS['fxptdata']['lvalues'][$mc][$i]['rk_ort1'].' &gt; '.$GLOBALS['fxptdata']['lvalues'][$mc][$i]['rk_ort2'].'</font> <i class="'.$ttcls.'">('.$GLOBALS['fxptdata']['lvalues'][$mc][$i]['rk_km'].' '.$km_str.')</i>';

				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['rechnungs_id']	= 0;
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['freigabe']		= 0;
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['icon_anzeigen']	= '~&nbsp;';
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['icon_aendern']	= '~&nbsp;';
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['icon_loeschen']	= '~&nbsp;';
			}

			$abrart=0;
			$rech_id=$GLOBALS['fxptdata']['lvalues'][$mc][$i]['rechnungs_id'];
			$nabr=$GLOBALS['fxptdata']['lvalues'][$mc][$i]['nicht_abrechenbar'];
			if($GLOBALS['fxptdata']['lvalues'][$mc][$i]['projekt_id'] > 0)
			{
				$abrart=db_value("SELECT abrechnungsart FROM budget_summe WHERE projekt_id=".$GLOBALS['fxptdata']['lvalues'][$mc][$i]['projekt_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
				if($abrart == FXP_INV_FIX)	// Festpreis
					$rech_id=-$rech_id;
			}

			// Zeitdaten
			if($GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeitart'] == FXP_TE_TIMEREC_SUM)
			{
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeitdaten']='~&sum;';
				$tm=meldung(3358,false);	// <b>&gt; $var1 additional entries exist &lt;</b><br /><i class=lightgrey>(Click to display the complete list in a new popup)</i>
				if($GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeiteinheit'] == $GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeit_id'])
				{
					$tt='';
					$tj='';
					$tc='help';
					$ec=1;
					$sql="SELECT ist_beginn,zeitaufwand,freigabe,rechnungs_id,nicht_abrechenbar FROM zeitdaten WHERE zeiteinheit=".$GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeiteinheit']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY ist_beginn";
					$mdt=db_values($sql,'*');
//fxDebug($mdt,$sql);
					if(fxIsArray($mdt))
					{
						$ec=sizeof($mdt);
						$zs=0;
						$zl=1;
						$ssd='';
						$sed='';
						foreach($mdt as $t)
						{
							if(!strlen((string)$ssd))
								$ssd=$t['ist_beginn'];
							$sed=$t['ist_beginn'];
						}
						$th='&sum; '.$ec.': '.get_entry($ssd,'datum');
						if(substr((string)$sed,0,8) != substr((string)$ssd,0,8))
							$th .= '-'.get_entry($sed,'datum');
						$tt  = '<b>'.$th.'</b><br /><br /><table border=0 cellpadding=1 cellspacing=2>';
						$tt .= '<tr><td class=crhd align=center><b>#</b></td><td class=crhd align=center>&nbsp;<b>'.$ldt.'</b>&nbsp;</td><td class=crhd align=center>&nbsp;<b>'.$lfr.'</b>&nbsp;</td><td class=crhd align=center>&nbsp;<b>'.$lab.'</b>&nbsp;</td><td class=crhd align=center>&nbsp;<b>'.$lzt.'</b>&nbsp;</td></tr>';
						foreach($mdt as $t)
						{
							$zs += $t['zeitaufwand'];

							if($t['freigabe'])
								$frg='<font color=#'.$txt_rl[$t['freigabe']]['col'].'>'.$txt_rl[$t['freigabe']]['txt'].'</font>';
							else
								$frg='<font class=grey>x</font>';

							if($t['rechnungs_id'])
								$abr='<font class=green>&#10004;</font>';
							else
								$abr='<font class=grey>x</font>';

							$tts='none';
							if($zl < 12)
								 $tts='table-row';
							else if($zl == 12)
							{
								$tt .= '<tr note style=display:table-row><td colspan=5 align=center bgcolor=#ffffff class=red><br />'.str_replace('$var1', $ec+1-$zl, $tm).'<br /><br /></td></tr>';
								$tc='pointer';
							}
							$tt .= '<tr style=display:'.$tts.'><td align=right>&nbsp;'.$zl.'.&nbsp;</td><td align=center>&nbsp;&nbsp;&nbsp;'.get_entry($t['ist_beginn'],'datum').'&nbsp;&nbsp;&nbsp;</td><td align=center>&nbsp;&nbsp;&nbsp;'.$frg.'&nbsp;&nbsp;&nbsp;</td><td align=center>&nbsp;&nbsp;&nbsp;'.$abr.'&nbsp;&nbsp;&nbsp;</td><td align=right>&nbsp;'.get_entry($t['zeitaufwand'],'zeitspanne').'&nbsp;</td></tr>';
							$zl++;
						}
						$tt .= '<tr><td class=rbt colspan=5 align=right><b>'.get_entry($zs,'zeitspanne').'</b>&nbsp;</td></tr>';
						$tt .= '</table>';
						$tj=' '.fxf_jsFunction('tooltipPopup', 'event');
					}
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeitdaten'] .= '&nbsp;<i class="s2 lightgrey" style="cursor:'.$tc.';" tooltip="'.$tt.'"'.$tj.'>'.$stt[0].'&nbsp;1/'.$ec.'</i>';
				}
				else
				{
					$sql="SELECT z.zeiteinheit,z.ist_beginn,t.taetigkeit_id FROM zeitdaten z, taetigkeiten t WHERE z.zeit_id=".$GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeiteinheit']." AND z.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND t.zeit_id=".$GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeiteinheit']." AND t.mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$mdt=db_values($sql);
//fxDebug($mdt,$sql);
					if(fxIsArray($mdt))
					{
						$GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeitdaten'] .= '&nbsp;<dont class="s2 lightgrey">&rarr;</font>';
						if($rights34)
							$GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeitdaten'] .= '<u id="trjl'.$GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeit_id'].'" name="trjl'.$GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeit_id'].'" class="fxfil" href="?tid='.$mdt['taetigkeit_id'].'" tr="34" style="font-size:10px;color:#969696;" tooltip="<b>'.$stt[1].'!</b><br>'.$stt[2].'">'.get_entry($mdt['ist_beginn'],'datum').'</u>';
						else
							$GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeitdaten'] .= '<i class="s2 lightgrey" style="cursor:help;" tooltip="<b>'.$stt[1].'!</b>">'.get_entry($mdt['ist_beginn'],'datum').'</i>';
					}
				}
			}
			else if($stime == $etime)
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeitdaten']='~';
			else
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeitdaten']='~<font class="'.$ttcls.'">'.$stime.'-'.$etime.'</font>';

			// Projektname
			$GLOBALS['fxptdata']['lvalues'][$mc][$i]['app_projektnummer']='';
			if($GLOBALS['fxptdata']['lvalues'][$mc][$i]['projekt_id'] < 0)
			{
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['name_projekt']=$gleitzeit;
				if($GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeitaufwand'])
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeitaufwand']=-$GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeitaufwand'];
			}
			else if($GLOBALS['fxptdata']['lvalues'][$mc][$i]['projekt_id'] < 1)
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['name_projekt']=$unbekannt;
			else
			{
				// App
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['app_projektnummer']=no_null($GLOBALS['fxptdata']['lvalues'][$mc][$i]['vorgangsnummer']);
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['app_projektname']=strtr($GLOBALS['fxptdata']['lvalues'][$mc][$i]['name_projekt'], $GLOBALS['caption_strtr']);

				$projektname=$GLOBALS['fxptdata']['lvalues'][$mc][$i]['name_projekt'];
				if(strlen((string)$projektname) > 36)
					$projektname=text_wrap($projektname,false,36).'&hellip;';
				$projektname=strtr($projektname, $GLOBALS['caption_strtr']);
				if($GLOBALS['fxptdata']['lvalues'][$mc][$i]['projekt_status'] != FXP_PS_ACTIVE)
					$projektname='<font class="darkred">'.$projektname.'</font>';
				if(strtolower((string)$GLOBALS['fxpglobals']['settings']['zeit_projektnr']) == 'ja')
					$projektname='<font class="grey">'.no_null($GLOBALS['fxptdata']['lvalues'][$mc][$i]['vorgangsnummer']).'</font> '.$projektname;
				else
					$projektname .= ' <font class="grey">['.no_null($GLOBALS['fxptdata']['lvalues'][$mc][$i]['vorgangsnummer']).']</font>';

				// Spaltenfarbe
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['name_projekt'] ='^class_rv'.(string)(($i%2) + 1);
				// Projektname
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['name_projekt'] .= '|~'.$projektname;
			}
			if(!isset($GLOBALS['fxptdata']['lvalues'][$mc][$i]['app_projektname']))
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['app_projektname']=$GLOBALS['fxptdata']['lvalues'][$mc][$i]['name_projekt'];

			// Leistungsart
			if($kr_aktiv && ($GLOBALS['fxptdata']['lvalues'][$mc][$i]['projekt_id'] > 0))
			{
				$sql ="SELECT s.kostenstelle_id, s.kostennummer, s.kostenbezeichnung, s.produktiv FROM kostenstellen s, kostenzuordnung z";
				$sql .= " WHERE z.id=".$GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeit_id']." AND z.kostenstellenart=".FXP_TE_TIMEREC." AND z.kostendefinition=903";
				$sql .= " AND s.kostenstelle_id=z.kostenstelle_id AND s.mandanten_id=z.mandanten_id AND s.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND s.mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$lainf=db_values($sql);
//fxDebug($lainf, $sql);
				if(fxIsArray($lainf))
				{
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['leistungsarten'] ='^class_rv'.(string)(($i%2) + 1);
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['leistungsarten'] .= '|~<span id="lad'.$i.'" tooltip="'.$lainf['kostenbezeichnung'].'">'.$lainf['kostennummer'].'</span>';
				}
				else
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['leistungsarten'] ='^class_rv'.(string)(($i%2) + 1).'|~&nbsp;';
			}

			// Icons
			$right=true;
			if($GLOBALS['fxptdata']['lvalues'][$mc][$i]['projekt_status'] != FXP_PS_ACTIVE)
				$right=false;
			if($right && ($GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeitart'] == FXP_TE_TIMEREC_SUM))
			{
				if(!$rights34 || ($tr != 34))
					$right=false;
			}
			if($right && ($GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeitart'] == FXP_TE_TIMEREC))
			{
				if(!$rights81 || ($tr != 81))
					$right=false;
			}
			if($right && (($GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeitart'] == FXP_TE_TRAVELEXP_TIMEREC) || ($GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeitart'] == FXP_TE_TRAVELEXP_TIMEREC)))
			{
				if(!$rights150 || ($tr != 150))
					$right=false;
			}
			if(!$right || ($rech_id > 0) || ($GLOBALS['fxptdata']['lvalues'][$mc][$i]['freigabe'] > ($GLOBALS['fxpglobals']['persdat']['role']+1)))
			{
				if($GLOBALS['app_wd'])
				{
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['icon_aendern']='~<a data-role="button" data-icon="gear" data-theme="c" class="ui-disabled" style="margin:0 0 6px 16px;">'.$app_lit_edt.'</a>';
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['icon_loeschen']='~<a data-role="button" data-icon="delete" data-theme="c" class="ui-disabled" style="margin:0 0 6px 0;">'.$app_lit_del.'</a>';
				}
				else
				{
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['icon_aendern']='~<img src="'.$GLOBALS['gfxpath'].'icch.png" class="ainactive">';
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['icon_loeschen']='~<img src="'.$GLOBALS['gfxpath'].'icde.png" class="ainactive">';
				}
			}
			else if($GLOBALS['app_wd'] && (!isset($GLOBALS['fxptdata']['lvalues'][$mc][$i]['icon_aendern']) || !strlen((string)$GLOBALS['fxptdata']['lvalues'][$mc][$i]['icon_aendern'])))
			{
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['icon_aendern']='~<a nid="btnedit" btn="action" tid="'.$GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeit_id'].'" data-role="button" data-icon="gear" data-theme="c" style="margin:0 0 6px 16px;">'.$app_lit_edt.'</a>';
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['icon_loeschen']='~<a nid="btndelete" btn="action" tid="'.$GLOBALS['fxptdata']['lvalues'][$mc][$i]['zeit_id'].'" data-role="button" data-icon="delete" data-theme="c" style="margin:0 0 6px 0;">'.$app_lit_del.'</a>';
			}
			if($GLOBALS['fxptdata']['lvalues'][$mc][$i]['projekt_id'] < 0)
			{
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['icon_aendern']='~|';
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['icon_loeschen']='~|';
			}

			// Freigabe (Status)
			$st='~<div style="width:38px;">';
			// ...Freigabe
			if($GLOBALS['fxptdata']['lvalues'][$mc][$i]['freigabe'])
				$st .= $ico_rl[$GLOBALS['fxptdata']['lvalues'][$mc][$i]['freigabe']];
			else
				$st .= $ico_leer;
			// ...Nicht verrechenbar/Bereits indirekt abgerechnet (Festpreis!)/Bereits abgerechnet
			if($rech_id)
			{
				$ari=abs($rech_id);
				if(!isset($rnarr[$ari]))
					$rnarr[$ari]=db_value("SELECT rechnn_f FROM rechnung WHERE rechnungs_id=".$ari." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
				if($rech_id > 0)
					$st .= str_replace('{RN}', $rnarr[$ari], $ico_rchng);
				else
					$st .= str_replace('{RN}', $rnarr[$ari], $ico_rchfp);
			}
			else if($nabr)
				$st .= $ico_nverr;
			else
				$st .= $ico_leer;
			$st .= '</div>';
			$GLOBALS['fxptdata']['lvalues'][$mc][$i]['status_benutzer']=$st;
			$i++;
		}
	}
	else
		$GLOBALS['fxptdata']['lvalues'][$mc]=true;

	// Prüfen, ob bereits Abwesenheitstag - falls ja, dann Warnhinweis anzeigen
	if($vb_pid)
	{
		$ad=okTimeRegistration(0,$vb_pid,$datum);
		if(!$err && !$Button_OK && strlen((string)$ad))
		{
			error_msg(3014, false, $ad);	// Ganztägige Abwesenheitstage an folgenden Tagen vorhanden:<br>$var1<br><br><font class=red>Es dürfen an diesen Tagen keine Zeiten erfasst werden!</font>
			$maske218_nv=true;
		}
	}

	// Datum
	$ay=(int)substr((string)$datum,0,4);
	$am=(int)substr((string)$datum,4,2);
	$ad=(int)substr((string)$datum,6,2);
	$aw=kw($datum);

	// Personenname in Überschrift einfügen
	$mhead=meldung(1190, false);
	$mhead=str_replace('$var1', $personenname, $mhead);

	// Wochentagsname in Überschrift einfügen
	$wochentag=fx_date('w', fx_mktime(12,0,0, $am,$ad,$ay));
	$wochentag=$wochentagsnamen[$wochentag]['tabwert'];
	$mhead=str_replace('$var2', $wochentag, $mhead);

	// Datum in Überschrift einfügen
	$mhead=str_replace('$var3', (string)(formatdatetime($GLOBALS['fxpglobals']['settings']['formatdatum'], $datum)), $mhead);

	// Sprung zur Materialverbrauchseingabe, sowie Kalendernavigation
	$maske217_head  = fieldLinkImage('new', 'img_mat', '?pers='.$vb_pid.'&gd='.substr((string)$datum,0,8), $ita[5].': <b>'.get_entry($datum,'datum').'</b>', 102, 'float:right;margin-right:6px;opacity:0.88;');
	$maske217_head .= addNavigation($ay,$am,$ad,0,'left').'<span class="maskht">'.$mhead.'</span>'.addNavigation($ay,$am,$ad,$ad);

	// --- Auswahlmaske füllen ---
	if(!$mst && ($Dummy == FXP_CHANGE))
	{
		$enddatum='';

		$Projekt_ID_dis=true;
		$Kunde_dis=true;
		$startdatum_dis=true;
		$enddatum_dis=true;
	}
}

// Wochen- oder Monatsübersicht
else if($gw || $gm)
{
	// Hiden
	$Datum_nv=true;

	// Nowrap
	$Tage_nw=true;
	$Taetigkeit_Text_nw=true;
	$SOLLStunden_nw=true;
	$ISTStunden_nw=true;
	$Delta_nw=true;

	if($gw)
	{
		$ad=(int)substr((string)$gw,10,2);
		$am=(int)substr((string)$gw,4,2);
		$ay=(int)substr((string)$gw,0,4);

		$von=substr('0000'.$ay,-4).substr('00'.$am,-2).substr('00'.$ad,-2).'000000';
		$bis=substr((string)time_add($von,6,'D'),0,8).'240000';
//fxDebug($gw,'$gw: $von='.$von.', $bis='.$bis);
	}
	else
	{
		$ad=1;
		$am=(int)substr((string)$gm,4,2);
		$ay=(int)substr((string)$gm,0,4);

		$von=substr((string)$gm,0,6).'01000000';
		$bis=substr((string)ultimo($von),0,8).'240000';
//fxDebug($gm,'$gm: $von='.$von.', $bis='.$bis);
	}

	// Variablen vorbereiten für Aufruf der Programmfunktion "Zeitdaten-Report"...
	$Button_Anzeigen=true;
	$Personen_ID=$vb_pid;
	$Anzeigeart=917;	// Tageweise
	$schaetzung=1;
	$mcb=$_maskcounter;
	$_maskcounter=$mnr_gw;

	// ...und Programmfunktion aufrufen...
	$GLOBALS['fxptdata']['lvalues'][$_maskcounter]=array();
	$GLOBALS['srd_mnr']=223;
	$GLOBALS['no_efield']=true;
	$req_fn=fxf_fn_reqFilename('std_rep_datum_pre', 57);
	if(strlen((string)$req_fn))
		require($req_fn);

	// ...und Ergebnis anpassen
	if(fxIsArray($GLOBALS['fxptdata']['lvalues'][$_maskcounter]))
	{
		$dd=$ad;
		foreach($GLOBALS['fxptdata']['lvalues'][$_maskcounter] as $ln => $la)
		{
			// Leerzeilen
			if($GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['tage'] == '~|')
			{
				if(!$ln)
					unset($GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]);
				else
				{
					$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['taetigkeit_text']='~|';	//'^class_transparent" style="border-top:1px solid blue;|~';
					$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['stundencheck']='~|';
					$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['icon_aendern']='~|';
				}
			}
			else
			{
				// Colspan-Zeile
				$cs=strpos($GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['tage'],'colspan="');
				if($cs)
					$cs=(int)substr((string)$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['tage'],$cs+9,2);

				// Übertrag
				if($cs > 3)
				{
					$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['tage']=strtr($GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['tage'],array('"12"'=>'"5"'));
					$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['icon_aendern']='^class_transparent|~';
				}
				else
				{
					$wdatum=makeDate($ay,$am,$dd, 0,0,0, true);
					$wday=substr((string)$wdatum,0,8);
//echo('$ay='.$ay.', $am='.$am.', $ad='.$ad.' -- $dd='.$dd.' -- $wdatum='.$wdatum.', $wday='.$wday.'<br />');

					// Spalte "Tätigkeit" oder "S-Chk" setzen
					if($cs)
						$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['stundencheck']=$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['datum'];
					else
						$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['taetigkeit_text']=$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['datum'];
					// "S-Chk" setzen
					if(!$cs)
					{
						if(fxIsArray($ia) && fxIsArray($ia[$vb_pid]) && isset($ia[$vb_pid][$wday]))
							$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['stundencheck']='~<img src="'.$GLOBALS['gfxpath'].'st_green.png">';
						else if($GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['sollstunden'] != '~')
						{
							if($wday < substr((string)$GLOBALS['datetime'],0,8))
								$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['stundencheck']='~<img src="'.$GLOBALS['gfxpath'].'st_red.png">';
							else
								$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['stundencheck']='~<img src="'.$GLOBALS['gfxpath'].'st_neutral.png">';
						}
						else
							$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['stundencheck']='~&nbsp;';
					}
					// Spalte "*" setzen=Link zum jeweiligen Tag
					if(!$cs)
					{
						if($GLOBALS['app_wd'])
						{
							if($GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['sollstunden'] != '~')
								$adt='a';
							else
								$adt='c';
							$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['icon_aendern']='~<a nid="btnweek" btn="jump" nmode="3" ndate="'.$wday.'" data-role="button" data-icon="arrow-r" data-theme="'.$adt.'" style="margin:0 0 6px 16px;">'.$app_lit_jpd.'</a>';
						}
						else
						{
							$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['icon_aendern']='^class_x|~'.fieldLinkImage('icon_aendern', 'icln', '?gd='.$wdatum, $ita[4].': <b>'.get_entry($wdatum,'datum').'</b>');
							// ...Zur Materialverbrauchseingabe wechseln
							if($rightmu)
								$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['icon_aendern'] .= fieldLinkImage('new', 'icmat', '?pers='.$vb_pid.'&gd='.substr((string)$wdatum,0,8), $ita[5].': <b>'.get_entry($wdatum,'datum').'</b>', 102, 'margin-left:6px;opacity:0.66;');
						}
						$dd++;
					}
					else
						$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['icon_aendern']='^class_transparent|~';
					// Datum hidden
					$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['datumsangabe']=$wdatum;
				}
			}
		}
//fxDebug($GLOBALS['fxptdata']['lvalues'][$mnr_gw], '$mnr_gw='.$mnr_gw);
	}

	// Masken-Override
	$wtid=1243;
	if($gw)
		$wtid=1240;
	// Personenname in Überschrift einfügen
	$mhead=meldung($wtid, false);
	$mhead=str_replace('$var1', $personenname, $mhead);

	if($gw)
	{
		// Kalenderwoche in Überschrift einfügen
		$kw  = meldung(1036);	// Kalenderwoche-Kurzzeichen "KW"
		$kw .= '&nbsp;'.substr((string)$gw,-2).',&nbsp;'.$ay;
	}
	else
	{
		// Monatsname in Überschrift einfügen
		$kw  = $monatsnamen[$am-1]['tabwert'];
		$kw .= ',&nbsp;'.$ay;
	}
	$mhead=str_replace('$var2',$kw,$mhead);
	if($gw)
		$maske223_head=addNavigation($ay,$am,0,0,'left').$mhead.addNavigation($ay,$am,$ad);
	else
		$maske223_head=addNavigation($ay,0,0,0,'left').$mhead.addNavigation($ay,$am);

	$maske223_al='left';
	$maske223_cl='all';
	$maske223_nl=true;

	$_maskcounter=$mcb;
}

// Jahresübersicht
else if($gj)
{
	$ay=(int)$gj;

	// Variablen vorbereiten für Aufruf der Programmfunktion "Zeitdaten-Report"...
	$Button_Anzeigen=true;
	$Personen_ID=$vb_pid;
	$Anzeigeart=918;	// Monatsweise
	$schaetzung=1;
	$von=$ay.'0101000000';
	$bis=$ay.'1231240000';
	$mcb=$_maskcounter;
	$_maskcounter=$mnr_gj;

	// ...und Programmfunktion aufrufen...
	$GLOBALS['fxptdata']['lvalues'][$_maskcounter]=array();
	$GLOBALS['srd_mnr']=226;
	$GLOBALS['no_efield']=true;
	$req_fn=fxf_fn_reqFilename('std_rep_datum_pre', 57);
	if(strlen((string)$req_fn))
		require($req_fn);

	// ...und Ergebnis anpassen
	if(fxIsArray($GLOBALS['fxptdata']['lvalues'][$_maskcounter]))
	{
		// Monatstage
		$mo_tage=array(1 => 31,28,31,30,31,30,31,31,30,31,30,31);
		$lj=fx_date('L', fx_mktime(12,0,0, 1,1,$ay));
		if($lj)
			$mo_tage[2]=29;
		$am=1;
		$ad=0;

		foreach($GLOBALS['fxptdata']['lvalues'][$_maskcounter] as $ln => $la)
		{
			// Spalte "Monatsname" setzen
			$t2m=$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['tage'];
			if(($t2m !== NULL) && strpos($t2m,'colspan="2"'))
				$t2m=strtr($t2m, array('colspan="2"'=>'colspan="1"', '"><!--'=>''));
			$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['monatsname']=$t2m;
			unset($GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['tage']);
			unset($GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['datum']);

			// Spalte "Tage" setzen
			$vdc=0;
			if(isset($la['arbeitstage']) && ($la['arbeitstage'] != '~|'))
			{
				$cls='';
				$csp=strpos($GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['monatsname'],'class="');
				if($csp)
				{
					$cep=strpos($GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['monatsname'],'"',$csp+7);
					$cls=substr((string)$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['monatsname'],$csp+7,$cep-$csp-7);
				}
				if(!strlen((string)$cls))
					$cls='lightergrey';
				$ptp=strpos($GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['monatsname'],'|~');
				if(!$ptp)
				{
					$vdc=$am;
					$ad += $mo_tage[$am];
					$am++;
					$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['tagesanzahl']='~&nbsp;<font class="'.$cls.'">'.get_entry($mo_tage[$vdc],'dezimal').'&nbsp;d</font>';
				}
				else
					$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['tagesanzahl']=substr((string)$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['monatsname'],0,$ptp+2).'&nbsp;'.get_entry($ad,'dezimal').'&nbsp;d';
			}

			// Spalte "*" setzen=Link zum jeweiligen Monat
			if($vdc)
			{
//<a nid="btnyear" btn="jump" nmode="5" ndate="{ndate}" data-role="button" data-icon="forward" data-theme="b" style="margin-top:16px;display:block;">'.$lit_arr["jump"].'</a>
				if($GLOBALS['app_wd'])
					$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['icon_aendern']='~<a nid="btnyear" btn="jump" nmode="5" ndate="'.$ay.substr('00'.$vdc,-2).'01" data-role="button" data-icon="arrow-r" data-theme="a" style="margin:0 0 6px 16px;">'.$app_lit_jpm.'</a>';
				else
					$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['icon_aendern']='^class_x|~'.fieldLinkImage('icon_aendern', 'icln', '?gm='.substr('0000'.$ay, -4).substr('00'.$vdc, -2).'00000000', $ita[2].': <b>'.$monatsnamen[$vdc-1]['tabwert'].' '.$ay);
			}
			else
				$GLOBALS['fxptdata']['lvalues'][$_maskcounter][$ln]['icon_aendern']='^class_transparent|~';
		}
//fxDebug($GLOBALS['fxptdata']['lvalues'][$mnr_gj], '$mnr_gj='.$mnr_gj);
	}

	// Masken-Override
	$mhead=meldung(1244, false);
	// Personenname in Überschrift einfügen
	$mhead=str_replace('$var1', $personenname, $mhead);
	// Jahr in Überschrift einfügen
	$mhead=str_replace('$var2', $ay, $mhead);
	// ...Navigation: Vorheriges / Nächstes Jahr
	$maske226_head=$mhead.addNavigation($ay);

	$_maskcounter=$mcb;
}
?>