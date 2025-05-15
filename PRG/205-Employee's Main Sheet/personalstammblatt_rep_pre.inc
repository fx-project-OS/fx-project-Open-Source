<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : personalstammblatt_rep_pre.inc                               //
// Version     : 24.1                                                         //
// Begin       : 2020-12-17                                                   //
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
 * PF 205: Employee's Main Sheet
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

if($Personen_ID && $Button_Anzeig)
{
	$mc=$GLOBALS['_maskcounter'];

	// Maskenfelder nicht umbrechen
	$nv_text1_nw=true;
	$nv_text2_nw=true;
	$nv_text3_nw=true;
	$nv_text4_nw=true;
	$nv_text5_nw=true;

	// Keine Spaltenüberschriften und Rahmen anzeigen
	$maske426_nch=true;
	$maske426_nbd=true;

	// Allgemeine Texte
	$firma=db_value("SELECT pname FROM personen WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND personentyp=274");

	// Währung
	$currency=$GLOBALS['fxpglobals']['settings']['waehrung'];
	$cst=darf_kosten_sehen();	// 0=None, 1=Intern, 2=Extern or 3=Both
	$redact='###';

	// Daten holen
	$personen_info=get_persondata($Personen_ID);
	$personen_sql="SELECT * FROM personen WHERE personen_id=".$Personen_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$personen_array=db_values($personen_sql);
	$madaten_sql="SELECT * FROM madaten WHERE personen_id=".$Personen_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$madaten_array=db_values($madaten_sql);
//fxDebug($madaten_array, '$madaten_array');

	$stdtag=0.00;
	if((int)$madaten_array['arbeitstage'] > 0)
		$stdtag=$madaten_array['wochenarbeitstd']/$madaten_array['arbeitstage'];

	// Summenliterale
	$lit_summe='<b>'.meldung(1999, true).'</b>';
	$lit_startsumme='<b>'.meldung(329, true).'</b>';
	$lit_zwischensumme='<b>'.meldung(1436, true).'</b>';
	$lit_gesamtsumme='<b>'.meldung(1555, true).'</b>';

	$lit_urluebertrag=meldung(2796,true);	// Urlaubsübertrag aus Vorjahr
	$lit_urlganspruch=meldung(2797,true);	// Gesamturlaubsanspruch für
	$lit_urltanspruch=meldung(2795,true);	// Teilurlaubsanspruch für
	$lit_urlrest=meldung(2799,true);		// Resturlaub bis Ende

	$txt_noentries='<i class=grey>('.meldung(2960, true).')</i>';	// Keine Einträge vorhanden!


	// Urlaubsberechnungsliterale
	$lva=explode('|', meldung(1887,false));	// No defined start value exists|Defined start value|Calculated intermediate value|Calculated new value
	$htx=explode('|', meldung(3397,false));	// Vacation carryover (from previous year)|Defined vacation per year (for whole year)|Missing months (entry/exit date)|Vacation taken|Remaining vacation (in current year)|Vacation carryover (for next year)

	$csa=array(
		'right'					=> '^col_?" align="right|~',
		'right_b'				=> '^col_?" align="right" style="border-top:1px solid #dddddd;padding-bottom:4px;|~',
		'zwischensumme_l'		=> '^col_?" style="border-top:1px solid #dddddd;padding-top:2px;padding-bottom:8px;color:#333333;font-weight:bold;|~',
		'zwischensumme_r'		=> '^col_?" align="right" style="border-top:1px solid #dddddd;padding-top:2px;padding-bottom:8px;color:#333333;|~',
		'gesamtsumme_l'			=> '^col_?" style="border-top:1px solid #dddddd;padding-top:4px;padding-bottom:8px;font-weight:bold;|~',
		'gesamtsumme_r'			=> '^col_?" align="right" style="border-top:1px solid #dddddd;padding-top:4px;padding-bottom:8px;|~',
		'urlaubuebertrag_l'		=> '^col_?" style="padding-top:8px;color:#006b9f;font-weight:bold;|~',
		'urlaubuebertrag_r'		=> '^col_?" align="right" style="padding-top:8px;color:#006b9f;font-weight:bold;|~',
		'urlaubuebertrag_b_l'	=> '^col_?" style="border-top:1px solid #dddddd;padding-top:8px;color:#006b9f;font-weight:bold;|~',
		'urlaubuebertrag_b_r'	=> '^col_?" align="right" style="border-top:1px solid #dddddd;padding-top:8px;color:#006b9f;font-weight:bold;|~',
		'urlaubuebertrag_a_l'	=> '^col_?" style="padding-top:8px;color:#aaaaaa;font-weight:bold;|~',
		'urlaubuebertrag_a_r'	=> '^col_?" align="right" style="padding-top:8px;color:#aaaaaa;font-weight:bold;|~',
		'urlaubanspruch_l'		=> '^col_?" style="padding-bottom:4px;color:#005077;font-weight:bold;|~',
		'urlaubanspruch_r'		=> '^col_?" align="right" style="padding-bottom:4px;color:#005077;font-weight:bold;|~',
		'urlaubanspruch_a_l'	=> '^col_?" style="padding-bottom:4px;color:#888888;font-weight:bold;|~',
		'urlaubanspruch_a_r'	=> '^col_?" align="right" style="padding-bottom:4px;color:#888888;font-weight:bold;|~',
		'resturlaub_l'			=> '^col_?" style="border-top:1px solid #dddddd;padding:8px 0;color:#009f6b;font-weight:bold;|~',
		'resturlaub_r'			=> '^col_?" align="right" style="border-top:1px solid #dddddd;padding:8px 0;color:#009f6b;font-weight:bold;|~',
		'header_l'				=> '^col_e8e8e8" style="padding:3px 2px;border-bottom:1px solid #ccc;font-weight:bold;|~',
		'header_r'				=> '^col_e8e8e8" align="right" style="padding:3px 2px;border-bottom:1px solid #ccc;font-weight:bold;|~'
	);

	$von=substr((string)$Zeitspanne_von, 0,8);
	$zvon='';
	if($madaten_array['maeintrittsdatum'] !== NULL)
		$zvon=substr((string)$madaten_array['maeintrittsdatum'], 0,8);
	if(!strlen((string)$von))
		$von=$zvon;
	else
		$zvon=max($zvon,$von);

	$bis=substr((string)$Zeitspanne_bis, 0,8);
	if(($madaten_array['maaustrittsdatum'] !== NULL) && strlen((string)$madaten_array['maaustrittsdatum']))
	{
		$zbis=substr((string)$madaten_array['maaustrittsdatum'], 0,8);
		if(!strlen((string)$bis))
			$bis=$zbis;
		else
			$zbis=min($zbis,$bis);
	}
	else if(!strlen((string)$bis))
	{
		$zbis=substr((string)$GLOBALS['datetime'],0,8);
		$bis=substr((string)$zbis,0,4).'1231';
		if((int)substr((string)$zbis,4,2) > 9)
			$bis=substr('0000'.(1+(int)substr((string)$bis, 0,4)), -4).'1231';
	}
	else
		$zbis=$bis;

	$von=min($von, $bis);
	$bis=max($von, $bis);
	$zvon=max($von, $zvon);
	$zbis=min($bis, $zbis);
//echo('$von='.$von.', $bis='.$bis.' -- $zvon='.$zvon.', $zbis='.$zbis.'<br>');

	// Vacation calculation (gets also holidays and picture indirectly)
	$vca=fxf_getVacCalc($Personen_ID);
//fxDebug($GLOBALS['__hda'],'All holidays: $GLOBALS[\'__hda\']', 0);
//fxDebug($GLOBALS['__ptp'][$Personen_ID],'Picture: \$GLOBALS[\'__ptp\']['.$Personen_ID.']', 0);

	// -> Name, Vorname
	$pi_name=$personen_array['pname'];
	$hd_name=get_text(14, '20');
	if($personen_array['vorname'])
	{
		$pi_name .= ', '.$personen_array['vorname'];
		$hd_name .= ', '.get_text(15, '20');
	}

	// -> Religion
	$rel=(int)$madaten_array['religion'];
	if($rel)
		$rel=meldung($madaten_array['religion'], true);
	else
		$rel='<i>('.meldung(681, true).')</i>';

	// Maskenüberschrift -> 1669: Personalstammblatt
	$maske426_head=$pi_name.' <i>('.get_entry($von, 'datum').'-'.get_entry($bis, 'datum').')</i>';

	$lc=0;

	// Picture
	if(fxIsArray($GLOBALS['__ptp'][$Personen_ID]) && !$GLOBALS['__ptp'][$Personen_ID]['def'] && strlen((string)$GLOBALS['__ptp'][$Personen_ID]['dsp']))
	{
		// Get biggest size
		if(isset($GLOBALS['__ptp'][$Personen_ID]['size_256']) && (strlen((string)$GLOBALS['__ptp'][$Personen_ID]['size_256']) > 0))
			$dwh='256';
		else if(isset($GLOBALS['__ptp'][$Personen_ID]['size_128']) && (strlen((string)$GLOBALS['__ptp'][$Personen_ID]['size_128']) > 0))
			$dwh='128';
		else if(isset($GLOBALS['__ptp'][$Personen_ID]['size_100']) && (strlen((string)$GLOBALS['__ptp'][$Personen_ID]['size_100']) > 0))
			$dwh='100';
		else
			$dwh='dsp';
		$txt='<img src="'.$GLOBALS['__ptp'][$Personen_ID]['dsp'].$GLOBALS['__ptp'][$Personen_ID]['mtime'].'"'.$GLOBALS['__ptp'][$Personen_ID]['size_'.$dwh].' align="right" style="max-height:'.$GLOBALS['__ptp'][$Personen_ID]['mxh'].'px;width:auto;margin-right:4px;border:1px solid white;border-right-color:black;border-bottom-color:black;border-radius:8px;box-shadow:3px 3px 6px rgba(0,0,0, 0.5);">';
		$atcr=10;
		if(($GLOBALS['fxpglobals']['tr'] != 204) && ($GLOBALS['fxpglobals']['tr'] != 205))
			$atcr=32;
		$txt .= '<img src="'.$GLOBALS['gfxpath'].'atc_c_80x80.png" style="position:absolute;right:'.$atcr.'px;top:-27px;">';
		$GLOBALS['fxptdata']['lvalues'][$mc][$lc]['nv_text1']='^col_*" nowrap colspan="5"><!--|~'.$txt;
		$GLOBALS['fxptdata']['lvalues'][$mc][$lc]['nv_text5']='~-->'.$txt;
		$lc++;

		$lc=leerzeile(426, $mc, $lc, true);
	}

	// Mitabeiterdaten
	lw_ueberschrift(get_text(1468, '20'));
	// Name, Vorname - Personen-ID (T264)
	lw_zeile($hd_name, $pi_name, get_text(264, '20'), $Personen_ID);
	// Geschlecht (T56) - Nummer (T37)
	lw_zeile(get_text(56, '20'), meldung($personen_array['geschlecht'], true), get_text(37, '20'), $personen_array['personen_initialen']);
	// Geburtsdatum (T52) - Religion (T1235)
	lw_zeile(get_text(52, '20'), get_entry($madaten_array['geburtsdatum'], 'datum'), get_text(1235, '20'), $rel);
	// Geburtsname (T57) - Schwerbehindert (T58)
	lw_zeile(get_text(57, '20'), $personen_array['geburtsname'], get_text(58, '20'), get_entry($madaten_array['schwerbehindert'], 'ja/nein'));
	// Interne Anmerkung (T110)
	if(($madaten_array['anmerkung'] !== NULL) && strlen((string)$madaten_array['anmerkung']))
	{
		$lc=leerzeile(426, $mc, $lc, true);

		$txt=text_wrap($madaten_array['anmerkung'],true,120);
		lw_zeile(get_text(110, '20'), '^col_?" nowrap colspan="4"><!--|~'.$txt, '~', '~-->'.$txt);
	}

	// Vertragsdaten
	if($Anz_Vd)
	{
		$lc=leerzeile(426, $mc, $lc, true);
		lw_ueberschrift(get_text(1470, '20'));
		// Personentyp (T30) - Pos. Stellenbeschreibung (T23)
		$inaktiv='';
		if($personen_array['inaktiv'])
			$inaktiv=' <i>('.meldung(2358, true).')</i>';
		lw_zeile(get_text(30, '20'), meldung($personen_array['personentyp'], true).$inaktiv, get_text(23, '20'), meldung($personen_array['pposition'], true));
		// Abteilung (T605) - Mobilbereich (T61)
		if(!$personen_array['abt_id'])
			$abt_array=array('abt_ebene'=>'0', 'abt_name'=>$firma);
		else
			$abt_array=db_values("SELECT abt_ebene, abt_name FROM abteilung WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND abt_id=".$personen_array['abt_id']);
		lw_zeile(get_text(605, '20'), trim((string)$abt_array['abt_ebene']).': '.$abt_array['abt_name'], get_text(61, '20'), meldung($madaten_array['mobilbereich'], true));
		// Eintrittsdatum (T54) - Vertragsstandort (T36)
		lw_zeile(get_text(54, '20'), get_entry($madaten_array['maeintrittsdatum'], 'datum'), get_text(36, '20'), $personen_array['vertragsstandort']);
		// Austrittsdatum (T55) - Einsatzort (T604)
		lw_zeile(get_text(55, '20'), get_entry($madaten_array['maaustrittsdatum'], 'datum'), get_text(604, '20'), $personen_array['einsatzort']);
		// Arbeitstage (T230) - Arbeitszeit (M1177)
		$p_at_zus='';
		$wotage=db_values("SELECT tabwert FROM wertetab WHERE id_feld=289 AND mandanten_id=0 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." ORDER BY position", '*');
		if(fxIsArray($wotage))
		{
			foreach($wotage as $ww)
			{
				if($madaten_array[strtolower((string)$ww['tabwert'])])
					$p_at_zus .= '-'.substr((string)$ww['tabwert'], 0, 2);
			}
			if($p_at_zus)
				$p_at_zus=' ('.substr((string)$p_at_zus, 1).')';
		}
		$p_at=get_entry($madaten_array['arbeitstage'], 'ganzzahl').$p_at_zus;
		$p_az=get_entry($madaten_array['soll_beginn'], 'zeit');
		if((int)$madaten_array['arbeitstage'] > 0)
		{
			$start=validate_entry($p_az, $verr, 'zeitspanne');
			$ende=$start + $stdtag*3600 + $madaten_array['pause'];
			$p_az .= ' - '.get_entry($ende, 'zeitspanne');
		}
		lw_zeile(get_text(230, '20'), $p_at, meldung(1177, true), $p_az);
		// Arbeitsstunden/Woche (T59) - Pause (T310)
		lw_zeile(get_text(59, '20'), get_entry($madaten_array['wochenarbeitstd'], 'dezimal'), get_text(310, '20'), get_entry($madaten_array['pause'], 'zeitspanne'));
		// Urlaub pro Jahr (T32) - Resturlaub (T49)
		$year=(int)substr((string)$GLOBALS['date'],0,4);
		if(fxIsArray($vca[$year]))
		{
			$ua=$vca[$year]['def_vacationperyear'];
			$ru=$vca[$year]['rem_vacationdays'];
		}
		else
		{
			$ua=0.00;
			$ru=0.00;
		}
		lw_zeile(get_text(32, '20').' ('.$year.')', get_entry($ua, 'dezimal'), get_text(49, '20').' (- '.get_entry($year.'1231240000', 'datum').')', get_entry($ru, 'dezimal'));
	}

	// Bankdaten
	if($Anz_Ba)
	{
		$lc=leerzeile(426, $mc, $lc, true);
		lw_ueberschrift(get_text(1469, '20'));
		// Bankleitzahl (T41) - Kontonummer (T40)
		lw_zeile(get_text(41, '20'), trim((string)$madaten_array['blz']), get_text(40, '20'), trim((string)$madaten_array['kontonummer']));
		// IBAN (T1911) - BIC (T1912)
		lw_zeile(get_text(1911, '20'), trim((string)$madaten_array['iban']), get_text(1912, '20'), trim((string)$madaten_array['bic']));
	}

	// Adressen: Hauptperson
	if($Anz_Adr)
	{
		$lc=leerzeile(426, $mc, $lc, true);
		lw_ueberschrift(get_text(1465, '20'));
		$data_found=false;
		$adr_array=psb_adresse($Personen_ID);
		if(fxIsArray($adr_array))
		{
			$data_found=true;
			$atyp=0;
			$lit=array('strasse' => get_text(20, '20'), 'land' => meldung(2117, true), 'ort' => get_text(24, '20').', '.get_text(35, '20'), 'staat' => get_text(34, '20'));
			foreach($adr_array as $aarr)
			{
				if(!fxIsArray($lnda))
					$lnda=array();
				if(!isset($lnda[$aarr['landkz']]))
					$lnda[$aarr['landkz']]=meldung($aarr['landkz'], true);
				if(!strlen((string)$lnda[$aarr['landkz']]))
					$lnda[$aarr['landkz']]='?';
				if($aarr['adresstyp'] != $atyp)
				{
					if($atyp)
						$lc=leerzeile(426, $mc, $lc, true);
					$atyp=$aarr['adresstyp'];
					$atue=meldung($atyp, true);
				}
				else
					$atue='';

				$staat='';
				if(($aarr['staat'] !== NULL) && strlen((string)$aarr['staat']))
					$staat=' ('.$aarr['staat'].')';
				lw_zeile($atue, $lnda[$aarr['landkz']].$staat, '~'.$aarr['strasse'], '~'.$aarr['plz'].' '.$aarr['ort']);
			}
		}
		if(!$data_found)
			lw_leer();
	}

	// Kommunikation: Hauptperson
	if($Anz_Kom)
	{
		$lc=leerzeile(426, $mc, $lc, true);
		lw_ueberschrift(get_text(1464, '20'));
		$data_found=false;
		$kom_array=psb_kommunikation($Personen_ID);
		if(fxIsArray($kom_array))
		{
			$ktyp=0;
			foreach($kom_array as $karr)
			{
				$ktxt=$karr['kommunikation_txt'];
				$atpos=strpos($ktxt, '@');
				if($atpos !== false)
					$ktxt='<a class="blue" href="mailto:'.$ktxt.'" target="_blank">'.$ktxt.'</a>';
				$webpos=strpos(strtolower((string)$ktxt), 'http');
				if($webpos === false)
					$webpos=strpos(strtolower((string)$ktxt), 'www.');
				if($webpos !== false)
				{
					if($GLOBALS['_elinks'])
						$ktxt='<a class="blue" href="'.$ktxt.'" target="_blank">'.$ktxt.'</a>';
					else
						$ktxt='<span class="blue">'.$ktxt.'</span>';
				}
				if($karr['anmerkung'])
					$kanm='<i>'.text_wrap($karr['anmerkung'],true,40).'</i>';
				else
					$kanm='';
				if($karr['kommunikationstyp'] != $ktyp)
				{
					$data_found=true;
					$ktyp=$karr['kommunikationstyp'];
					$ktue=meldung($ktyp, true);
				}
				else
					$ktue='';
				lw_zeile('', $ktue, '~'.$ktxt, '~'.$kanm);
			}
		}
		if(!$data_found)
			lw_leer();
	}

	// Fähigkeiten
	if($Anz_Fae)
	{
		$lc=leerzeile(426, $mc, $lc, true);
		lw_ueberschrift(get_text(1467, '20'));
		$data_found=false;
		$fsql="SELECT maart_id, mafaehigkeit_id, dauer, kommentar, bewertung FROM ma_art_faehigkeiten WHERE personen_id=".$Personen_ID." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].") ORDER BY maart_id, mafaehigkeit_id";
		$f_array=db_values($fsql, '*');
		if(fxIsArray($f_array))
		{
			$fart=0;
			$jahre=meldung(919, true);	// Jahr(e)
			$bew_array=array();
			foreach($f_array as $farr)
			{
				$ftxt=db_value("SELECT mafaehigkeit_name FROM ma_faehigkeit WHERE mafaehigkeit_id=".$farr['mafaehigkeit_id']." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].") AND id_sprache=".$GLOBALS['fxpglobals']['lang']);
				if(!isset($bew_array[$farr['bewertung']]))
					$bew_array[$farr['bewertung']]=trim(meldung($farr['bewertung'], true));
				$fzus=get_entry($farr['dauer'], 'dezimal').' '.$jahre;
				if(strlen((string)$bew_array[$farr['bewertung']]))
					$fzus .= '<div style="float:right;">['.$bew_array[$farr['bewertung']].']</div>';
				if($farr['maart_id'] != $fart)
				{
					$fart=$farr['maart_id'];
					$faue=db_value("SELECT maart_name FROM ma_art WHERE maart_id=".$fart." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].") AND id_sprache=".$GLOBALS['fxpglobals']['lang']);
				}
				if(strlen((string)$faue) && strlen((string)$ftxt))
					$nf_array[$faue][$ftxt]=$fzus;
			}
			if(fxIsArray($nf_array))
			{
				ksort($nf_array,SORT_STRING);
				foreach($nf_array as $maart => $nfarr)
				{
					ksort($nfarr,SORT_STRING);
					$mfc=0;
					foreach($nfarr as $mafae => $faean)
					{
						if(!$mfc)
						{
							$data_found=true;
							lw_zeile('', $maart, '~'.$mafae, '~<i>'.$faean.'</i>');
						}
						else
							lw_zeile('|', '~', '~'.$mafae, '~<i>'.$faean.'</i>');
						$mfc++;
					}
				}
			}
		}
		if(!$data_found)
			lw_leer();
	}

	// Arbeitszeiterfassung (Zeitdaten, Urlaub, Krankheit + Schulung)
	$Anz_Abw=false;
	if($Anz_Url || $Anz_Kr || $Anz_Schu)
		$Anz_Abw=true;
	if($Anz_Arb || $Anz_Url || $Anz_Kr || $Anz_Schu)
	{
		$sondertage=get_sondertage($von, $bis, $Personen_ID, 0, false, false);
//fxDebug($sondertage,'$sondertage', 0);

		$oop=overtime_outpayment($Personen_ID, $zvon, $zbis);
//fxDebug($oop,$zvon.'-'.$zbis, 0);

		$aze=array(
			'arb' => array('lit'=>get_text(1492, '20'),								'md'=>$Anz_Arb, 'art'=>0),
			'ueb' => array('lit'=>meldung(1943, false),								'md'=>$Anz_Arb, 'art'=>0),

			'gla' => array('lit'=>meldung(FXP_AT_SLIDINGDAYREQUEST, false),			'md'=>$Anz_Arb, 'art'=>FXP_AT_SLIDINGDAYREQUEST),
			'glt' => array('lit'=>meldung(FXP_AT_SLIDINGDAY, false),				'md'=>$Anz_Arb, 'art'=>FXP_AT_SLIDINGDAY),

			'abw' => array('lit'=>meldung(FXP_AT_ABSENT, false),					'md'=>$Anz_Abw, 'art'=>FXP_AT_ABSENT),

			'ura' => array('lit'=>meldung(FXP_AT_VACATIONREQUEST, false),			'md'=>$Anz_Url, 'art'=>FXP_AT_VACATIONREQUEST),
			'url' => array('lit'=>meldung(FXP_AT_VACATION, false),					'md'=>$Anz_Url, 'art'=>FXP_AT_VACATION),

			'sua' => array('lit'=>meldung(FXP_AT_SPECIALVACATIONREQUEST, false),	'md'=>$Anz_Url, 'art'=>FXP_AT_SPECIALVACATIONREQUEST),
			'sur' => array('lit'=>meldung(FXP_AT_SPECIALVACATION, false),			'md'=>$Anz_Url, 'art'=>FXP_AT_SPECIALVACATION),

			'krm' => array('lit'=>meldung(FXP_AT_SICKCALL, false),					'md'=>$Anz_Kr, 'art'=>FXP_AT_SICKCALL),
			'kra' => array('lit'=>meldung(FXP_AT_SICK, false),						'md'=>$Anz_Kr, 'art'=>FXP_AT_SICK),

			'sca' => array('lit'=>meldung(FXP_AT_TRAININGREQUEST, false),			'md'=>$Anz_Schu, 'art'=>FXP_AT_TRAININGREQUEST),
			'sch' => array('lit'=>meldung(FXP_AT_TRAINING, false),					'md'=>$Anz_Schu, 'art'=>FXP_AT_TRAINING)
		);
		$lwuz='';
		foreach($aze as $azv => $azl)
		{
			if(($azv != 'ueb') && ($azv != 'gla') && ($azv != 'ura') && ($azv != 'sua') && ($azv != 'krm') && ($azv != 'sca') && $azl['md'])
				$lwuz .= ', '.$azl['lit'];
		}

		$lc=leerzeile(426, $mc, $lc, true);
		lw_ueberschrift(meldung(733, true).' ('.substr((string)$lwuz, 2).')');

		$data_found=false;

		$monat=db_values("SELECT tabwert FROM wertetab WHERE id_feld=62 AND mandanten_id=0 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." ORDER BY position", '*');

		// Zeitdaten + Überstundenauszahlung:
		if($aze['arb']['md'])
		{
			psb_zeitdaten($Personen_ID, $zvon, $zbis);
			$st=getPersonWorkdata($Personen_ID, $zvon, $zbis);

			lw_zeile($csa['header_l'].$aze['arb']['lit'], $csa['header_l'], $csa['header_l'], $csa['header_l']);
			if(sizeof($zdat))
			{
				$display=array();
				$gesamt=0;
				foreach($zdat as $zd)
				{
					if($zd['projekt_id'] >= 0)
					{
						if($aze['arb']['md'] == 2541)	// Summen
							$k=substr((string)$zd['ist_beginn'], 0, 4);
						else
							$k=substr((string)$zd['ist_beginn'], 0, 6);
						if(!isset($display[$k]['von']) || (isset($display[$k]['von']) && ($display[$k]['von'] > $zd['ist_beginn'])))
							$display[$k]['von']=$zd['ist_beginn'];
						if(!isset($display[$k]['bis']) || (isset($display[$k]['bis']) && ($display[$k]['bis'] < $zd['ist_ende'])))
							$display[$k]['bis']=$zd['ist_ende'];
						$display[$k]['zeit'] += $zd['zeitaufwand'];
						$gesamt += $zd['zeitaufwand'];
					}
				}
				ksort($display,SORT_STRING);
//fxDebug($display, '$display');

				$zc=0;
				$zwischen=0;
				$zwc=0;
				$zcmax=sizeof($display)-1;
				foreach($display as $k => $dd)
				{
					if(!$zc)
						$jahr=(int)substr((string)$k, 0, 4);
					else
					{
						if($aze['arb']['md'] != 2541)	// Details
						{
							$njahr=(int)substr((string)$k, 0, 4);
							if($jahr != $njahr)
							{
								// Zwischensumme
								lw_zeile('|', $csa['zwischensumme_r'].$lit_zwischensumme.'&nbsp;', $csa['zwischensumme_l'].'&nbsp;'.$jahr, $csa['zwischensumme_r'].get_entry($zwischen/3600, 'dezimal').' h&nbsp;');
								$jahr=$njahr;
								$zwischen=0;
								$zwc++;
							}
						}
					}
					$zwischen += $dd['zeit'];
					if($aze['arb']['md'] == 2541)	// Summen
						$txt2=$lit_summe.' '.$k;
					else
						$txt2=$monat[(int)substr((string)$k, 4) - 1]['tabwert'].' '.substr((string)$k, 0, 4);
					if(substr((string)$dd['von'], 0, 8) == substr((string)$dd['bis'], 0, 8))
						$txt3='~'.get_entry($dd['von'], 'datum');
					else
						$txt3='~'.get_entry($dd['von'], 'datum').'-'.get_entry($dd['bis'], 'datum');
					lw_zeile('|', $csa['right'].$txt2, $txt3, $csa['right'].get_entry($dd['zeit']/3600, 'dezimal').' h');
					$zc++;
				}
				// Zwischensumme
				if($zwc && $zwischen && $jahr)
					lw_zeile('|', $csa['zwischensumme_r'].$lit_zwischensumme.'&nbsp;', $csa['zwischensumme_l'].'&nbsp;'.$jahr, $csa['zwischensumme_r'].get_entry($zwischen/3600, 'dezimal').' h&nbsp;');
				// Gesamtsumme
				lw_zeile($csa['gesamtsumme_l'].'&nbsp;', $csa['gesamtsumme_r'].$lit_gesamtsumme.'&nbsp;', $csa['gesamtsumme_l'].'&nbsp;'.get_entry($zvon, 'datum').'-'.get_entry($zbis, 'datum'), $csa['gesamtsumme_r'].get_entry($gesamt/3600, 'dezimal').' h&nbsp;');
			}
			else
				lw_leer();

			// Überstundenauszahlung
			$lc=leerzeile(426, $mc, $lc, true);
			lw_zeile($csa['header_l'].$aze['ueb']['lit'], $csa['header_l'], $csa['header_l'], $csa['header_l']);
			if(fxIsArray($oop))
			{
				$display=array();
				$gesamt=0;
				foreach($oop as $odate => $odata)
				{
					if($aze['arb']['md'] == 2541)	// Summen
						$k=substr((string)$odate, 0, 4);
					else
						$k=substr((string)$odate, 0, 6);

					if(!isset($display[$k]['von']) || (isset($display[$k]['von']) && ($display[$k]['von'] > $odate)))
						$display[$k]['von']=$odate;
					if(!isset($display[$k]['bis']) || (isset($display[$k]['bis']) && ($display[$k]['bis'] < $odate)))
						$display[$k]['bis']=$odate;

					$display[$k]['sec'] += $odata['sec'];
					$display[$k]['txt']=$odata['txt'];

					$gesamt += $odata['sec'];
				}
				ksort($display,SORT_STRING);
//fxDebug($display, '$display');

				$zc=0;
				$zwischen=0;
				$zwc=0;
				$zcmax=sizeof($display)-1;
				foreach($display as $k => $dd)
				{
					if(!$zc)
						$jahr=(int)substr((string)$k, 0, 4);
					else
					{
						if($aze['arb']['md'] != 2541)	// Details
						{
							$njahr=(int)substr((string)$k, 0, 4);
							if($jahr != $njahr)
							{
								// Zwischensumme
								lw_zeile('|', $csa['zwischensumme_r'].$lit_zwischensumme.'&nbsp;', $csa['zwischensumme_l'].'&nbsp;'.$jahr, $csa['zwischensumme_r'].get_entry($zwischen/3600, 'dezimal').' h&nbsp;');
								$jahr=$njahr;
								$zwischen=0;
								$zwc++;
							}
						}
					}
					$zwischen += $dd['zeit'];
					if($aze['arb']['md'] == 2541)	// Summen
						$txt2=$lit_summe.' '.$k;
					else
						$txt2=$monat[(int)substr((string)$k, 4) - 1]['tabwert'].' '.substr((string)$k, 0, 4);
					if($aze['arb']['md'] != 2541)	// Details
					{
						$txt3='~'.get_entry($dd['von'], 'datum');
						if(strlen((string)$dd['txt']))
							$txt3 .= '&nbsp;<span id="dummy" class="grey" tooltip="'.$dd['txt'].'" style="cursor:help;">&#10004;</span>';
					}
					else if(substr((string)$dd['von'], 0, 8) == substr((string)$dd['bis'], 0, 8))
						$txt3='~'.get_entry($dd['von'], 'datum');
					else
						$txt3='~'.get_entry($dd['von'], 'datum').'-'.get_entry($dd['bis'], 'datum');
					lw_zeile('|', $csa['right'].$txt2, '~'.$txt3, $csa['right'].get_entry($dd['sec']/3600, 'dezimal').' h');
					$zc++;
				}
				// Zwischensumme
				if($zwc && $zwischen && $jahr)
					lw_zeile('|', $csa['zwischensumme_r'].$lit_zwischensumme.'&nbsp;', $csa['zwischensumme_l'].'&nbsp;'.$jahr, $csa['zwischensumme_r'].get_entry($zwischen/3600, 'dezimal').' h&nbsp;');
				// Gesamtsumme
				lw_zeile($csa['gesamtsumme_l'].'&nbsp;', $csa['gesamtsumme_r'].$lit_gesamtsumme.'&nbsp;', $csa['gesamtsumme_l'].'&nbsp;'.get_entry($zvon, 'datum').'-'.get_entry($zbis, 'datum'), $csa['gesamtsumme_r'].get_entry($gesamt/3600, 'dezimal').' h&nbsp;');
			}
			else
				lw_leer();

			$data_found=true;
		}

		// Gleittag, Abwesend, Urlaub, Sonderurlaub, Schulung + Krankheit
		foreach($aze as $kurz => $aza)
		{
			if($aza['md'] && $aza['art'])
			{
				if($data_found)
					$lc=leerzeile(426, $mc, $lc, true);
				$data_found=true;

				$display=array();
				$gesamt=0.00;
				if(fxIsArray($sondertage))
				{
					foreach($sondertage as $sdatum => $sarray)
					{
						foreach($sarray as $styp => $sdata)
						{
							if($styp == $aza['art'])
							{
								if($aza['md']%2 == 1)	// Summen
									$k=substr((string)$sdatum, 0, 4);
								else
									$k=substr((string)$sdatum, 0, 6);

								if(!isset($display[$k]))
									$display[$k]=array('von'=>$sdatum.'000000', 'bis'=>$sdatum.'240000', 'zeit'=>$sdata[-1]['dauer']);
								else
								{
									$display[$k]['bis']=$sdatum.'240000';
									$display[$k]['zeit'] += $sdata[-1]['dauer'];
								}
								$gesamt += $sdata[-1]['dauer'];
							}
						}
					}
				}

				if($aza['art'] == FXP_AT_VACATION)
				{
					$syear=(int)substr((string)$zvon,0,4);
					$eyear=(int)substr((string)$zbis,0,4);
					for($year=$syear; $year<=$eyear; $year++)
						$display[$year.'99']=true;
					ksort($display,SORT_STRING);
				}

				if(fxIsArray($display))
				{
//fxDebug($display,'$display', 0);
					$eyear=(int)substr((string)$zbis,0,4);
					$display['9999']=true;

					$zc=0;
					$zwischen=0;
					$oldtxt2='';
					$oldyear=0;
					$hl='';
					$hr='';
					foreach($display as $k => $dd)
					{
						$year=(int)substr((string)$k,0,4);
						$month=(int)substr((string)$k,4,2);

						if(!$zc)
							lw_zeile($csa['header_l'].$aza['lit'], $csa['header_l'], $csa['header_l'], $csa['header_l']);

						$ueb='|';

						// Zwischensumme
						if($oldyear && ($year > $oldyear))
						{
							if($aza['md']%2 == 0)	// Details
							{
								$hd='<table width=100% border=0 cellspacing=0 cellpadding=0><tr><td align="right"><b>'.get_entry($zwischen, 'dezimal').'&nbsp;d</b>&nbsp;</td>';
								if($st > 0.00)
									$hd .= '<td align="center" width="24">&nbsp;=&nbsp;</td><td align="right" width="88"><b>'.get_entry($zwischen*$st, 'dezimal').'&nbsp;h</b>&nbsp;</td>';
								$hd .= '</tr></table>';
								lw_zeile($csa['zwischensumme_r'], $csa['zwischensumme_r'].$lit_zwischensumme.'&nbsp;', $csa['zwischensumme_l'].'&nbsp;'.$oldyear, $csa['zwischensumme_r'].$hd);
							}
							$zwischen=0;

							// Resturlaub
							if(($aza['art'] == FXP_AT_VACATION) && ($oldyear == $eyear) && fxIsArray($vca[$oldyear]))
							{
								$hl='&nbsp;'.$oldyear;
								$hr='<table border=0 cellspacing=0 cellpadding=0><tr><td align="right">'.get_entry($vca[$oldyear]['rem_vacationdays'], 'dezimal').'&nbsp;d&nbsp;</td></tr></table>';
							}
						}

						// Urlaubsübertrag aus Vorjahr + Gesamtanspruch für aktuelles Jahr
						if(($year != $oldyear) && fxIsArray($vca[$year]) && ($aza['md']%2 == 0))
						{
							if($aza['art'] == FXP_AT_VACATION)
							{
								// ...Vacation carry (from previous year)
								lw_zeile('~<b>'.$year.'</b>', '~<i>'.$lva[$vca[$year]['type']].'</i>', '~'.$htx[0], $csa['right'].get_entry($vca[$year]['vacationcarry'],'decimal').'&nbsp;d');
								// ...Defined vacation per year (for this whole year)
								lw_zeile('|', '~', '~+ '.$htx[1], $csa['right'].get_entry($vca[$year]['def_vacationperyear'],'decimal').'&nbsp;d');
								// ...Missing months (entry/exit date)
								if($vca[$year]['calc_vacationperyear'] < $vca[$year]['def_vacationperyear'])
									lw_zeile('|', '~', '~- '.$htx[2], $csa['right'].get_entry($vca[$year]['calc_vacationperyear']-$vca[$year]['def_vacationperyear'],'decimal').'&nbsp;d');
								// ...Entitled vacation days (for this year)
								lw_zeile('|', '~', $csa['right_b'].'~', $csa['right_b'].'<b>'.get_entry($vca[$year]['ent_vacationdays'],'decimal').'&nbsp;d</b>&nbsp;');
							}
							else
								lw_zeile('~<b>'.$year.'</b>', '~', '~', '~');
						}

						if(($year < 9999) && ($month < 99))
						{
							$zwischen += $dd['zeit'];
							if($aza['md']%2 == 1)	// Summen
							{
								$txt2=$csa['right'].$lit_summe;
								$txt3='~'.$k;
							}
							else
							{
								$txt2=$csa['right'].$monat[(int)substr((string)$k, 4, 2) - 1]['tabwert'].' '.substr((string)$k, 0, 4);
								if($oldtxt2 == $txt2)
									$txt2='&nbsp;';
								if(substr((string)$dd['von'], 0, 8) == substr((string)$dd['bis'], 0, 8))
									$txt3='~'.get_entry($dd['von'], 'datum');
								else
									$txt3='~'.get_entry($dd['von'], 'datum').'-'.get_entry($dd['bis'], 'datum');
							}
							$hd='<table width=100% border=0 cellspacing=0 cellpadding=0><tr><td align="right">'.get_entry($dd['zeit'], 'dezimal').'&nbsp;d</td>';
							if($st > 0.00)
								$hd .= '<td align="center" width="24">&nbsp;=&nbsp;</td><td align="right" width="88">'.get_entry($dd['zeit']*$st, 'dezimal').'&nbsp;h</td>';
							$hd .= '</tr></table>';
							lw_zeile($ueb, $txt2, $txt3, $csa['right'].$hd);
							if($txt2 != '&nbsp;')
								$oldtxt2=$txt2;
							$zc++;
						}

						$oldyear=$year;
					}

					// Gesamtsumme
					$hd='<table width=100% border=0 cellspacing=0 cellpadding=0><tr><td align="right"><b>'.get_entry($gesamt, 'dezimal').'&nbsp;d</b>&nbsp;</td>';
					if($st > 0.00)
						$hd .= '<td align="center" width="24">&nbsp;=&nbsp;</td><td align="right" width="88"><b>'.get_entry($gesamt*$st, 'dezimal').'&nbsp;h</b>&nbsp;</td>';
					$hd .= '</tr></table>';
					lw_zeile($csa['gesamtsumme_l'].'&nbsp;', $csa['gesamtsumme_r'].$lit_gesamtsumme.'&nbsp;', $csa['gesamtsumme_l'].'&nbsp;'.get_entry($zvon, 'datum').'-'.get_entry($zbis, 'datum'), $csa['gesamtsumme_r'].$hd);

					// Resturlaub
					if(strlen((string)$hl) && strlen((string)$hr))
						lw_zeile($csa['resturlaub_l'], $csa['resturlaub_r'].$lit_urlrest.'&nbsp;', $csa['resturlaub_l'].$hl, $csa['resturlaub_r'].$hr);
				}
				else
				{
					lw_zeile($csa['header_l'].$aza['lit'], $csa['header_l'], $csa['header_l'], $csa['header_l']);
					lw_leer();
				}
			}
		}

		// Aufgabenbericht + Überstundenbericht
		if($aze['arb']['md'])
		{
			psb_zeitdaten($Personen_ID, $zvon, $zbis);
			if(sizeof($zdat))
			{
//fxDebug($zdat, '$zdat');

				$an=array(0 => '<i class="grey">('.meldung(681, false).')</i>');		// (Unbekannt)
				$ab=array();

				foreach($zdat as $z)
				{
					$pid=(int)$z['projekt_id'];
					if($pid >= 0)	// Gleittage=-1 nicht berücksichtigen
					{
						if(!isset($an[$pid]))
						{
							$sql="SELECT vorgangsnummer, name_projekt FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
							$tmp=db_values($sql);
							if(fxIsArray($tmp))
							{
								if(strtolower((string)$GLOBALS['fxpglobals']['settings']['zeit_projektnr']) == 'ja')
									$an[$pid]='<b>'.no_null($tmp['vorgangsnummer']).'</b> - '.$tmp['name_projekt'];
								else
									$an[$pid]='<b>'.$tmp['name_projekt'].'</b> ('.no_null($tmp['vorgangsnummer']).')';
							}
							else
								$an[$pid]='{'.$pid.'}';
						}

						$ek=round_hset($Personen_ID, $z['projekt_id'], round($z['kosten_ek']*3600.00/$z['zeitaufwand'], 2), false, false);
						$vk=round_hset($Personen_ID, $z['projekt_id'], round($z['kosten_vk']*3600.00/$z['zeitaufwand'], 2), false, true);
//echo($an[$pid].' -- ek='.$ek.', vk='.$vk.'<br>');

						$id=$an[$pid].'|'.$ek.'|'.$vk;

						if(!isset($ab[$id]))
							$ab[$id]=array('vn'=>$an[$pid], 'sek'=>$z['zeitaufwand'], 'sek_nv'=>0.00, 'ek'=>$ek, 'vk'=>$vk);
						else
							$ab[$id]['sek'] += $z['zeitaufwand'];
						if(!$vk || $z['nicht_abrechenbar'])
							$ab[$id]['sek_nv'] += $z['zeitaufwand'];
					}
				}

				ksort($ab,SORT_STRING);
//fxDebug($ab, '$ab');

				// Aufgabenbericht
				$lc=leerzeile(426, $mc, $lc, true);
				lw_ueberschrift(meldung(2846, true).' <i>('.get_entry($zvon, 'datum').'-'.get_entry($zbis, 'datum').')</i>');	// Aufgabenbericht

				$lit_aufg='&nbsp;'.meldung(1475, false);												// Aufgaben
				$lit_ekvk='<table border=0 cellspacing=0 cellpadding=0><tr><td align="right">'.get_text(805, '5').'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td><td>&nbsp;&nbsp;&nbsp;&nbsp;/</td><td align="right" width="84">'.get_text(806, '5').'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr></table>';	// EK / VK
				$lit_sanz=meldung(2847, false).'&nbsp;';												// (n. abr.) Stundenanzahl
				lw_zeile($csa['header_l'].$lit_aufg, $csa['header_l'], $csa['header_r'].$lit_ekvk, $csa['header_r'].$lit_sanz);

				$ssek=0.00;
				foreach($ab as $ai => $ad)
				{
					$ssek += $ad['sek'];
					$ssek_nv += $ad['sek_nv'];
					if(($cst == 1) || ($cst == 3))
						$vint=get_entry($ad['ek'], 'dezimal');
					else
						$vint=$redact;
					if(($cst == 2) || ($cst == 3))
						$vext=get_entry($ad['vk'], 'dezimal');
					else
						$vext=$redact;
					$ev ='<table border=0 cellspacing=0 cellpadding=0><tr>';
					$ev .= '<td align="right">'.$vint.'&nbsp;'.$currency.'/h</td>';
					$ev .= '<td>&nbsp;&nbsp;&nbsp;&nbsp;/</td>';
					$ev .= '<td align="right" width="80">'.$vext.'&nbsp;'.$currency.'/h</td>';
					$ev .= '</tr></table>';
					$sa ='<table border=0 cellspacing=0 cellpadding=0><tr>';
					$sa .= '<td align="right">('.get_entry($ad['sek_nv']/3600.00, 'dezimal').'&nbsp;h)</td>';
					$sa .= '<td align="right" width="99">'.get_entry($ad['sek']/3600.00, 'dezimal').'&nbsp;h</td>';
					$sa .= '</tr></table>';
					lw_zeile('^col_?" nowrap colspan="2" align="left"><!--|~'.$ad['vn'], '~-->'.$ad['vn'], $csa['right'].$ev, $csa['right'].$sa);
				}

				// Gesamtsumme
				$txt=$lit_gesamtsumme;
				$gs ='<table border=0 cellspacing=0 cellpadding=0><tr>';
				$gs .= '<td align="right">('.get_entry($ssek_nv/3600.00, 'dezimal').'&nbsp;h)</td>';
				$gs .= '<td align="right" width="99">'.get_entry($ssek/3600.00, 'dezimal').'&nbsp;h&nbsp;</td>';
				$gs .= '</tr></table>';
				lw_zeile($csa['gesamtsumme_l'].$txt, $csa['gesamtsumme_l'].'&nbsp;', $csa['gesamtsumme_l'].'&nbsp;', $csa['gesamtsumme_r'].$gs);

				// Überstundenbericht
				$lc=leerzeile(426, $mc, $lc, true);
				lw_ueberschrift(meldung(2848, true).' <i>('.get_entry($zvon, 'datum').'-'.get_entry($zbis, 'datum').')</i>');	// Überstundenbericht

				// Startbilanz...
				// ...entspr. Eintrittsdatum
				$sb_startdatum=trim(substr((string)$madaten_array['maeintrittsdatum'], 0, 8));
				$sb_summe	=0;
				// ...entspr. Zeiterfassungs-Übertrags-Startwert
				$sql="SELECT zeitaufwand, ist_beginn FROM zeitdaten WHERE zeitart=105 AND personen_id=".$Personen_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$tmp=db_values($sql);
//fxDebug($tmp, $sql);
				if(fxIsArray($tmp))
				{
					$sbj=(int)substr((string)$tmp['ist_beginn'], 0, 4);
					$sbm=(int)substr((string)$tmp['ist_beginn'], 4, 2);
					$sbm++;
					if($sbm > 12)
					{
						$sbm=1;
						$sbj++;
					}

					$sb_startdatum=substr('0000'.$sbj, -4).substr('00'.$sbm, -2).'01';
					$sb_summe	=(int)$tmp['zeitaufwand'];
				}
				// ...entspr. Mandanteneinstellung
				else if($GLOBALS['fxpglobals']['settings']['zk_ueb_jahr'])
				{
					$datum=substr('0000'.$GLOBALS['fxpglobals']['settings']['zk_ueb_jahr'], -4).substr('00'.$GLOBALS['fxpglobals']['settings']['zk_ueb_monat'], -2).'01';
					if(($datum <= $zbis) && ($datum > $sb_startdatum))
						$sb_startdatum=$datum;
				}
//echo('PRE: $sb_startdatum='.$sb_startdatum.', $sb_summe='.$sb_summe.'<br>');

				if($sb_startdatum <= $zvon)
				{
					$dvon='';
					$dbis='';
					$rvon=$zvon;
					$rbis=$zbis;

					$sb_summe	=summe_zeitkonto($Personen_ID, substr((string)$zvon, 0, 6));
					$sb_startdatum=$zvon;
				}
				else if($sb_startdatum > $zbis)
				{
					$dvon=$zvon;
					$dbis=$zbis;
					$rvon='';
					$rbis='';
				}
				else
				{
					$dvon=$zvon;
					$dbis=fx_date("Ymd", fx_mktime(12, 0, 0, (int)substr((string)$sb_startdatum, 4, 2), (int)substr((string)$sb_startdatum, 6, 2)-1, (int)substr((string)$sb_startdatum, 0, 4)));
					$rvon=$sb_startdatum;
					$rbis=$zbis;
				}
				$datage=0;
				if(strlen((string)$dvon))
					$datage=ceil(countWeekdays($dvon, $dbis, (int)$madaten_array['sonntag'], (int)$madaten_array['montag'], (int)$madaten_array['dienstag'], (int)$madaten_array['mittwoch'], (int)$madaten_array['donnerstag'], (int)$madaten_array['freitag'], (int)$madaten_array['samstag']));
				$ratage=0;
				if(strlen((string)$rvon))
					$ratage=ceil(countWeekdays($rvon, $rbis, (int)$madaten_array['sonntag'], (int)$madaten_array['montag'], (int)$madaten_array['dienstag'], (int)$madaten_array['mittwoch'], (int)$madaten_array['donnerstag'], (int)$madaten_array['freitag'], (int)$madaten_array['samstag']));
//echo('PST: $sb_startdatum='.$sb_startdatum.', $sb_summe='.$sb_summe.'<br>');
//echo('$dvon='.$dvon.', $dbis='.$dbis.' =&gt; '.$datage.' d -- $rvon='.$rvon.', $rbis='.$rbis.' =&gt; '.$ratage.' d<br>');

				$dftage=0.00;
				$rftage=0.00;
				if(fxIsArray($GLOBALS['__hda']))
				{
					$wa=array((int)$madaten_array['sonntag'], (int)$madaten_array['montag'], (int)$madaten_array['dienstag'], (int)$madaten_array['mittwoch'], (int)$madaten_array['donnerstag'], (int)$madaten_array['freitag'], (int)$madaten_array['samstag']);
					foreach($GLOBALS['__hda'] as $hyear => $ha)
					{
						foreach($ha as $fdatum => $farray)
						{
							if(($fdatum >= $zvon) && ($fdatum <= $zbis) && ($farray['typ'] > 0.00))
							{
								$wt=getWeekday($fdatum);
								if($wa[$wt])	// Arbeitstag
								{
									if(strlen((string)$dbis) && ($fdatum <= $dbis))
										$dftage += $farray['typ'];
									else
										$rftage += $farray['typ'];
								}
							}
						}
					}
				}

				$dabtage=0.00;
				$dutage =0.00;
				$dsutage=0.00;
				$dstage =0.00;
				$dktage =0.00;

				$rabtage=0.00;
				$rutage =0.00;
				$rsutage=0.00;
				$rstage =0.00;
				$rktage =0.00;

				if(fxIsArray($sondertage))
				{
//fxDebug($sondertage,'$sondertage');
					foreach($sondertage as $sdatum => $sarray)
					{
						if(($sdatum >= $zvon) && ($sdatum <= $zbis))
						{
							foreach($sarray as $styp => $sdata)
							{
								if($styp == FXP_AT_ABSENT)
								{
									if(strlen((string)$dbis) && ($sdatum <= $dbis))
										$dabtage += $sdata[-1]['dauer'];
									else
										$rabtage += $sdata[-1]['dauer'];
								}
								else if(($styp == FXP_AT_VACATIONREQUEST) || ($styp == FXP_AT_VACATION))
								{
									if(strlen((string)$dbis) && ($sdatum <= $dbis))
										$dutage += $sdata[-1]['dauer'];
									else
										$rutage += $sdata[-1]['dauer'];
								}
								else if(($styp == FXP_AT_SPECIALVACATIONREQUEST) || ($styp == FXP_AT_SPECIALVACATION))
								{
									if(strlen((string)$dbis) && ($sdatum <= $dbis))
										$dsutage += $sdata[-1]['dauer'];
									else
										$rsutage += $sdata[-1]['dauer'];
								}
								else if(($styp == FXP_AT_TRAININGREQUEST) || ($styp == FXP_AT_TRAINING))
								{
									if(strlen((string)$dbis) && ($sdatum <= $dbis))
										$dstage += $sdata[-1]['dauer'];
									else
										$rstage += $sdata[-1]['dauer'];
								}
								else if(($styp == FXP_AT_SICKCALL) || ($styp == FXP_AT_SICK))
								{
									if(strlen((string)$dbis) && ($sdatum <= $dbis))
										$dktage += $sdata[-1]['dauer'];
									else
										$rktage += $sdata[-1]['dauer'];
								}
							}
						}
					}
				}

				$duasek=0;
				$ruasek=0;

				if(fxIsArray($oop))
				{
//fxDebug($oop, $zvon.'-'.$zbis);
					foreach($oop as $odatum => $oarray)
					{
						if(strlen((string)$dbis) && ($odatum <= $dbis))
							$duasek += $oarray['sec'];
						else
							$ruasek += $oarray['sec'];
					}
				}
//echo('$dftage='.$dftage.', $rftage='.$rftage.' -- $dabtage='.$dabtage.' -- $dutage='.$dutage.', $rutage='.$rutage.' -- $dsutage='.$dsutage.', $rsutage='.$rsutage.' -- $dstage='.$dstage.', $rstage='.$rstage.' -- $dktage='.$dktage.', $rktage='.$rktage.' -- $duasek='.$duasek.', $ruasek='.$ruasek.'<br>');

				$dsek=0;
				$rsek=0;
				foreach($zdat as $z)
				{
					if($z['projekt_id'] >= 0)
					{
						$zdatum=substr((string)$z['ist_beginn'], 0, 8);
						if(strlen((string)$dbis) && ($zdatum <= $dbis))
							$dsek += $z['zeitaufwand'];
						else
							$rsek += $z['zeitaufwand'];
					}
				}
//echo('$dsek='.$dsek.', $rsek='.$rsek.'<br>');

				$lit_azeit='&nbsp;'.get_text(1916, '20');	// Anzeigeart Zeitraum
				$lit_tanz=get_text(921, '20').'&nbsp;';		// Tagesanzahl
				$lit_sanz=get_text(1877,'20').'&nbsp;';		// Stundenanzahl
				lw_zeile($csa['header_l'].$lit_azeit, $csa['header_l'], $csa['header_r'].$lit_tanz, $csa['header_r'].$lit_sanz);

				// 2849: Arbeitstage|Feiertage|Abwesenheitstage|Urlaubstage|Sonderurlaubstage|Schulungstage|Krankheitstage|Startübertrag|Soll-Stunden|Ist-Stunden|Minusstunden|Überstunden|Deltastunden
				$txt_arr=explode('|', meldung(2849, false));
				$txt_atage='<b>'.$txt_arr[0].'</b>&nbsp;';		// Arbeitstage
				$txt_ftage='-&nbsp;'.$txt_arr[1].'&nbsp;';		// - Feiertage
				$txt_abtage='-&nbsp;'.$txt_arr[2].'&nbsp;';		// - Abwesenheitstage
				$txt_utage='-&nbsp;'.$txt_arr[3].'&nbsp;';		// - Urlaubstage
				$txt_sutage='-&nbsp;'.$txt_arr[4].'&nbsp;';		// - Sonderurlaubstage
				$txt_stage='-&nbsp;'.$txt_arr[5].'&nbsp;';		// - Schulungstage
				$txt_ktage='-&nbsp;'.$txt_arr[6].'&nbsp;';		// - Krankheitstage
				$txt_sbil='<b>'.$txt_arr[7].'</b>&nbsp;';		// Startübertrag
				$txt_ist=$txt_arr[9].'&nbsp;';					// Ist-Stunden
				$txt_uaus='-&nbsp;'.meldung(1943).'&nbsp;';		// - Überstundenauszahlung
				$txt_soll='-&nbsp;'.$txt_arr[8].'&nbsp;';		// - Soll-Stunden

				$drarr=array('d', 'r');
				foreach($drarr as $dr)
				{
					if((($dr == 'd') && strlen((string)$dvon)) || (($dr == 'r') && strlen((string)$rvon)))
					{
						if(($dr == 'r') && strlen((string)$dvon))
							$lc=leerzeile(426, $mc, $lc, true);

						if($dr == 'd')
						{
							$st=getPersonWorkdata($Personen_ID, $dvon, $dbis);
//fxDebug($st,'Dummy: $st');
							$zeitraum=get_entry($dvon, 'datum').'-'.get_entry($dbis, 'datum');
							$kla='(';
							$klz=')';
						}
						else
						{
							$st=getPersonWorkdata($Personen_ID, $rvon, $rbis);
//fxDebug($st,'Actual: $st');
							$zeitraum=get_entry($rvon, 'datum').'-'.get_entry($rbis, 'datum');
							$kla='';
							$klz='';
						}

						// Zeitraum
						if(strlen((string)$dvon) && strlen((string)$rvon))
							lw_zeile('~<u>'.$zeitraum.'</u>', '', '~', '');
						// Arbeitstage
						$h='';
						if($st > 0.00)
							$h=$kla.get_entry(${$dr.'atage'}*$st, 'dezimal').'&nbsp;h'.$klz;
						lw_zeile('~<i class="darkgrey">'.$kla.get_entry($st,'dezimal').'&nbsp;h/d'.$klz.'</i>', '~'.$txt_atage, $csa['right'].$kla.get_entry(${$dr.'atage'}, 'dezimal').'&nbsp;d'.$klz, $csa['right'].$h);
						// - Feiertage
						$h='';
						if($st > 0.00)
							$h='-&nbsp;'.$kla.get_entry(${$dr.'ftage'}*$st, 'dezimal').'&nbsp;h'.$klz;
						lw_zeile('|', '~'.$txt_ftage, $csa['right'].'-&nbsp;'.$kla.get_entry(${$dr.'ftage'}, 'dezimal').'&nbsp;d'.$klz, $csa['right'].$h);
						// - Abwesenheitstage
						$h='';
						if($st > 0.00)
							$h='-&nbsp;'.$kla.get_entry(${$dr.'abtage'}*$st, 'dezimal').'&nbsp;h'.$klz;
						lw_zeile('|', '~'.$txt_abtage, $csa['right'].'-&nbsp;'.$kla.get_entry(${$dr.'abtage'}, 'dezimal').'&nbsp;d'.$klz, $csa['right'].$h);
						// - Urlaubstage
						$h='';
						if($st > 0.00)
							$h='-&nbsp;'.$kla.get_entry(${$dr.'utage'}*$st, 'dezimal').'&nbsp;h'.$klz;
						lw_zeile('|', '~'.$txt_utage, $csa['right'].'-&nbsp;'.$kla.get_entry(${$dr.'utage'}, 'dezimal').'&nbsp;d'.$klz, $csa['right'].$h);
						// - Sonderurlaubstage
						$h='';
						if($st > 0.00)
							$h='-&nbsp;'.$kla.get_entry(${$dr.'sutage'}*$st, 'dezimal').'&nbsp;h'.$klz;
						lw_zeile('|', '~'.$txt_sutage, $csa['right'].'-&nbsp;'.$kla.get_entry(${$dr.'sutage'}, 'dezimal').'&nbsp;d'.$klz, $csa['right'].$h);
						// - Schulungstage
						$h='';
						if($st > 0.00)
							$h='-&nbsp;'.$kla.get_entry(${$dr.'stage'}*$st, 'dezimal').'&nbsp;h'.$klz;
						lw_zeile('|', '~'.$txt_stage, $csa['right'].'-&nbsp;'.$kla.get_entry(${$dr.'stage'}, 'dezimal').'&nbsp;d'.$klz, $csa['right'].$h);
						// - Krankheitstage
						$h='';
						if($st > 0.00)
							$h='-&nbsp;'.$kla.get_entry(${$dr.'ktage'}*$st, 'dezimal').'&nbsp;h'.$klz;
						lw_zeile('|', '~'.$txt_ktage, $csa['right'].'-&nbsp;'.$kla.get_entry(${$dr.'ktage'}, 'dezimal').'&nbsp;d'.$klz, $csa['right'].$h);

						if($st > 0.00)
						{

							// Zwischensumme Soll-Stunden
							$txt='&nbsp;'.$lit_zwischensumme.'&nbsp;'.$txt_arr[8];
							$soll=(${$dr.'atage'}-${$dr.'ftage'}-${$dr.'abtage'}-${$dr.'utage'}-${$dr.'sutage'}-${$dr.'stage'}-${$dr.'ktage'})*$st;
							lw_zeile('|', $csa['zwischensumme_l'].$txt, $csa['zwischensumme_r'].$kla.get_entry($soll/$st, 'dezimal').'&nbsp;d'.$klz.'&nbsp;', $csa['zwischensumme_r'].$kla.get_entry($soll, 'dezimal').'&nbsp;h'.$klz.'&nbsp;');

							$lc=leerzeile(426, $mc, $lc, true);

							// Startbilanz
							$uebertrag=0;
							$txt=$txt_sbil;
							if($dr == 'r')
							{
								$uebertrag=$sb_summe/3600.00;
								$txt .= '(- '.get_entry($sb_startdatum, 'datum').')&nbsp;';
							}
							$vz='';
							if($uebertrag < 0)
								$vz='-';
							$sb=abs($uebertrag);
							if($dr == 'r')
								lw_zeile('|', '~'.$txt, $csa['right'].$vz.'&nbsp;'.$kla.get_entry($sb/$st, 'dezimal').'&nbsp;d'.$klz, $csa['right'].$vz.'&nbsp;'.$kla.get_entry($sb, 'dezimal').'&nbsp;h'.$klz);
							// Ist-Stunden
							$vz='';
							if($dr == 'r')
								$vz='+&nbsp;';
							$ist=${$dr.'sek'}/3600.00;
							lw_zeile('|', '~'.$vz.$txt_ist, $csa['right'].$vz.$kla.get_entry($ist/$st, 'dezimal').'&nbsp;d'.$klz, $csa['right'].$vz.$kla.get_entry($ist, 'dezimal').'&nbsp;h'.$klz);
							// - Überstundenauszahlung
							$uaus=${$dr.'uasek'}/3600.00;
							lw_zeile('|', '~'.$txt_uaus, $csa['right'].'-&nbsp;'.$kla.get_entry($uaus/$st, 'dezimal').'&nbsp;d'.$klz, $csa['right'].'-&nbsp;'.$kla.get_entry($uaus, 'dezimal').'&nbsp;h'.$klz);
							// - Soll-Stunden
							lw_zeile('|', '~'.$txt_soll, $csa['right'].'-&nbsp;'.$kla.get_entry($soll/$st, 'dezimal').'&nbsp;d'.$klz, $csa['right'].'-&nbsp;'.$kla.get_entry($soll, 'dezimal').'&nbsp;h'.$klz);

							// Überstunden/Minusstunden
//echo('$uebertrag='.$uebertrag.', $ist='.$ist.', $uaus='.$uaus.', $soll='.$soll);
							$uestd=$uebertrag+$ist-$uaus-$soll;
							if($dr == 'r')
							{
								if($uestd >= 0.00)
								{
									$cls=' class="green"';
									$txt=$txt_arr[11];		// Überstunden
								}
								else
								{
									$cls=' class="red"';
									$txt=$txt_arr[10];		// Minustunden
								}
							}
							else
							{
								$txt=$txt_arr[12];			// Deltastunden
								if($uestd >= 0.00)
									$cls=' class="lightgreen"';
								else
									$cls=' class="lightred"';
							}
							lw_zeile('|', $csa['gesamtsumme_l'].'<span'.$cls.'>'.$txt.'</span>', $csa['gesamtsumme_r'].'<span'.$cls.'>'.$kla.get_entry($uestd/$st, 'dezimal').'&nbsp;d'.$klz.'&nbsp;</span>', $csa['gesamtsumme_r'].'<span'.$cls.'>'.$kla.get_entry($uestd, 'dezimal').'&nbsp;h'.$klz.'&nbsp;</span>');
						}
					}
				}
			}
		}
	}

	// Personenhierarchie
	if($Anz_PersHier)
	{
		$lc=leerzeile(426, $mc, $lc, true);
		lw_ueberschrift(get_text(1466, '20'));
		$data_found=false;

		// Vorgesetzter
		if($personen_array['elter'] && ($personen_array['elter'] != $Personen_ID))
		{
			$data_found=true;
			$vorg_array=db_values("SELECT pname, vorname, pposition, abt_id FROM personen WHERE personen_id=".$personen_array['elter']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);

			// -> Name, Vorname
			$vorg_name=$vorg_array['pname'];
			if($vorg_array['vorname'])
				$vorg_name .= ', '.$vorg_array['vorname'];

			// Abteilung
			if(!$vorg_array['abt_id'])
				$abt_array=array('abt_ebene'=>'0', 'abt_name'=>$firma);
			else
				$abt_array=db_values("SELECT abt_ebene, abt_name FROM abteilung WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND abt_id=".$vorg_array['abt_id']);

			// Personenposition
			$ppos='';
			if($vorg_array['pposition'])
				$ppos=trim(meldung($vorg_array['pposition'], true));
			if(!strlen((string)$ppos))
				$ppos='&nbsp;';

			// Vorgesetzter (T770)
			lw_zeile(get_text(770, '20'), '~'.$vorg_name, '~<i>'.$ppos.'</i>', '~<i>'.trim((string)$abt_array['abt_ebene']).': '.$abt_array['abt_name'].'</i>');
		}

		// Untergeordnete
		$le=db_values("SELECT personen_id FROM personen WHERE elter=".$Personen_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (personentyp=".FXP_PT_EMPLOYEE." OR personentyp=".FXP_PT_CONTRACTOR.") AND personen_id<>".$Personen_ID." AND (inaktiv IS NULL OR inaktiv=0)", '*');
		if(fxIsArray($le))
		{
			$pn=array();
			foreach($le as $pida)
			{
				$pid=(int)$pida['personen_id'];
				$vorg_array=db_values("SELECT pname, vorname, pposition, abt_id FROM personen WHERE personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
				// -> Name, Vorname
				$vorg_name=$vorg_array['pname'];
				if($vorg_array['vorname'])
					$vorg_name .= ', '.$vorg_array['vorname'];
				$pn[$vorg_name]['ppos']=$vorg_array['pposition'];
				$pn[$vorg_name]['abt']=$vorg_array['abt_id'];
			}
			if(fxIsArray($pn))
			{
				$data_found=true;
				ksort($pn,SORT_STRING);
				// Untergeordnete (M2371)
				$upz=0;
				foreach($pn as $name => $value)
				{
					// Abteilung
					if(!$value['abt_id'])
						$abt_array=array('abt_ebene'=>'0', 'abt_name'=>$firma);
					else
						$abt_array=db_values("SELECT abt_ebene, abt_name FROM abteilung WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND abt_id=".$value['abt']);

					// Personenposition
					$ppos='';
					if($value['ppos'])
						$ppos=trim(meldung($value['ppos'], true));
					if(!strlen((string)$ppos))
						$ppos='&nbsp;';

					if($upz)
						$h='|';
					else
						$h=meldung(2371,true);
					lw_zeile($h, '~'.$name, '~<i>'.$ppos.'</i>', '~<i>'.trim((string)$abt_array['abt_ebene']).': '.$abt_array['abt_name'].'</i>');

					$upz++;
				}
			}
		}
		if(!$data_found)
			lw_leer();
	}

	// (nicht) verrechenbare Stunden
	if($nicht_abrechenbar)
	{
		$lc=leerzeile(426, $mc, $lc, true);
		lw_ueberschrift(meldung(2786, true).' <i>('.get_entry($zvon, 'datum').'-'.get_entry($zbis, 'datum').')</i>');	// Auflistung verrechenbarer und nicht verrechenbarer Stunden

		psb_zeitdaten($Personen_ID, $zvon, $zbis);

		if(sizeof($zdat))
		{
			$va=array();
			$ka=array();
			$na=array();

			foreach($zdat as $z)
			{
				if($z['projekt_id'] > -1)	// Gleittage nicht berücksichtigen
				{
					if(($z['stdek'] !== NULL) && strlen((string)$z['stdek']))
					{
						$ek=$z['stdek'];
						$vk=$z['stdvk'];
					}
					else
					{
						$ek=round_hset($Personen_ID, $z['projekt_id'], round($z['kosten_ek']*3600.00/$z['zeitaufwand'], 2), false, false);
						$vk=round_hset($Personen_ID, $z['projekt_id'], round($z['kosten_vk']*3600.00/$z['zeitaufwand'], 2), false, true);
						$z['stdek']=$ek;
						$z['stdvk']=$vk;
					}
//echo($z['projekt_id'].': ek='.$ek.', vk='.$vk.'<br>');

					$nv=0;
					$gvk=$z['kosten_vk'];
					if($z['nicht_abrechenbar'])
					{
						$nv=1;
						$gvk =0.00;
					}

					// Auflistung verrechenbarer und nicht verrechenbarer Stunden
					$vid=$ek.'|'.$vk.'|'.$nv;
					if(!isset($va[$vid]))
						$va[$vid]=array('sek'=>$z['zeitaufwand'], 'gek'=>$z['kosten_ek'], 'gvk'=>$gvk);
					else
					{
						$va[$vid]['sek'] += $z['zeitaufwand'];
						$va[$vid]['gek'] += $z['kosten_ek'];
						$va[$vid]['gvk'] += $gvk;
					}

					// Verrechenbare Stunden, d.h. Zeichen "nicht anrechenbar" nicht gesetzt und VK>0
					if(!$nv && $vk)
					{
						$kid=$vk;
						if(!isset($ka[$kid]))
							$ka[$kid]=array('sek'=>$z['zeitaufwand'], 'gvk'=>$gvk);
						else
						{
							$ka[$kid]['sek'] += $z['zeitaufwand'];
							$ka[$kid]['gvk'] += $gvk;
						}
					}

					// Nicht verrechenbare Stunden, d.h. Zeichen "nicht anrechenbar" gesetzt oder VK=0
					if($nv || !$vk)
					{
						$nid=$vk;
						if(!isset($na[$nid]))
							$na[$nid]=$z['zeitaufwand'];
						else
							$na[$nid] += $z['zeitaufwand'];
					}
				}
			}

			if(!sizeof($va))
			{
				$va['0|0']=array('sek'=>0, 'gek'=>0.00, 'gvk'=>0.00);
				$ka[0]=array('sek'=>0, 'gvk'=>0.00);
			}
			else
			{
				ksort($va,SORT_STRING);
				ksort($ka,SORT_STRING);
			}
//fxDebug($va, '$va');
//fxDebug($ka, '$ka');

			$lit_ek	=get_text(805, '5');												// EK
			$lit_vk	=get_text(806, '5');												// VK
			$lit_sanz=get_text(1877, '20');												// Stundenanzahl
			$lit_netto=get_text(1072, '20').' ('.$lit_vk.' '.get_text(1976, '10').')';	// Nettobetrag (VK Verrech.)
			lw_zeile($csa['header_r'].$lit_ek.'&nbsp;', $csa['header_r'].$lit_vk.'&nbsp;', $csa['header_r'].$lit_sanz.'&nbsp;', $csa['header_r'].$lit_netto.'&nbsp;');

			$ssek=0.00;
			$sgek=0.00;
			$sgvk=0.00;
			$lcnt=0;
			foreach($va as $ekvk => $vdat)
			{
				$evk=explode('|', $ekvk);
				$ek =(double)$evk[0];
				$vk =(double)$evk[1];
				$nv =(int)$evk[2];

				$ssek += $vdat['sek'];
				$sgek += $vdat['gek'];
				$sgvk += $vdat['gvk'];

				if(($cst == 1) || ($cst == 3))
					$vint=get_entry($ek, 'dezimal');
				else
					$vint=$redact;
				if(($cst == 2) || ($cst == 3))
				{
					$vext1=get_entry($vk, 'dezimal');
					$vext2=get_entry($vdat['gvk'], 'dezimal');
				}
				else
				{
					$vext1=$redact;
					$vext2=$redact;
				}

				$cls='';
				if($nv || !$vk)
					$cls=' class="red"';

				lw_zeile($csa['right'].'<span'.$cls.'>'.$vint.'&nbsp;'.$currency.'/h</span>', $csa['right'].'<span'.$cls.'>'.$vext1.'&nbsp;'.$currency.'/h</span>', $csa['right'].'<span'.$cls.'>'.get_entry($vdat['sek']/3600.00, 'dezimal').'&nbsp;h</span>', $csa['right'].'<span'.$cls.'>'.$vext2.'&nbsp;'.$currency.'</span>');

				$lcnt++;
			}
			// Summenzeile
			if(($cst == 1) || ($cst == 3))
				$vint=get_entry($sgek*3600.00/$ssek, 'dezimal');
			else
				$vint=$redact;
			if(($cst == 2) || ($cst == 3))
			{
				$vext1=get_entry($sgvk*3600.00/$ssek, 'dezimal');
				$vext2=get_entry($sgvk, 'dezimal');
			}
			else
			{
				$vext1=$redact;
				$vext2=$redact;
			}
			lw_zeile($csa['gesamtsumme_r'].'&Delta;&nbsp;'.$vint.'&nbsp;'.$currency.'/h&nbsp;', $csa['gesamtsumme_r'].'&Delta;&nbsp;'.$vext1.'&nbsp;'.$currency.'/h&nbsp;', $csa['gesamtsumme_r'].get_entry($ssek/3600.00, 'dezimal').'&nbsp;h&nbsp;', $csa['gesamtsumme_r'].$vext2.'&nbsp;'.$currency.'</b>&nbsp;');

			$lc=leerzeile(426, $mc, $lc, true);

			$ssek=0.00;
			$sgvk=0.00;
			foreach($ka as $vk => $kdat)
			{
				$ssek += $kdat['sek'];
				$sgvk += $kdat['gvk'];

				if(($cst == 2) || ($cst == 3))
				{
					$vext1=get_entry($vk, 'dezimal');
					$vext2=get_entry($kdat['gvk'], 'dezimal');
				}
				else
				{
					$vext1=$redact;
					$vext2=$redact;
				}

				// 2787: Summe verrechenbarer Stunden mit
				$txt='&nbsp;'.meldung(2787, true).'&nbsp;'.$lit_vk.'&nbsp;=&nbsp;&nbsp;'.$vext1.'&nbsp;'.$currency.'/h';
				lw_zeile('^col_?" nowrap colspan="2"><!--|~'.$txt, '~-->'.$txt, $csa['right'].get_entry($kdat['sek']/3600.00, 'dezimal').'&nbsp;h', $csa['right'].$vext2.'&nbsp;'.$currency);
			}
			// Summenzeile
			$gvsek=$ssek;
			if(($cst == 2) || ($cst == 3))
				$vext=get_entry($sgvk, 'dezimal');
			else
				$vext=$redact;
			$txt=meldung(2788, true);	// 2788: Gesamtsumme verrechenbarer Stunden
			lw_zeile('*gesamtsumme_l', '&nbsp;'.$txt, $csa['gesamtsumme_r'].get_entry($ssek/3600.00, 'dezimal').'&nbsp;h&nbsp;', $csa['gesamtsumme_r'].$vext.'&nbsp;'.$currency.'&nbsp;');


			if(!sizeof($na))
				$na[0]=0;
			else
				ksort($na,SORT_STRING);
//fxDebug($na, '$na');

			$lc=leerzeile(426, $mc, $lc, true);

			$ssek=0.00;
			foreach($na as $vk => $sek)
			{
				$ssek += $sek;
				if(($cst == 2) || ($cst == 3))
					$vext=get_entry($vk, 'dezimal');
				else
					$vext=$redact;

				// 2789: Summe nicht verrechenbarer Stunden mit
				$txt='&nbsp;'.meldung(2789, true).'&nbsp;'.$lit_vk.'&nbsp;=&nbsp;&nbsp;'.$vext.'&nbsp;'.$currency.'/h';
				lw_zeile('^col_?" nowrap colspan="2"><!--|~'.$txt, '~-->'.$txt, $csa['right'].get_entry($sek/3600.00, 'dezimal').'&nbsp;h', $csa['right'].get_entry(0, 'dezimal').'&nbsp;'.$currency);
			}
			// Summenzeile
			$gnsek=$ssek;
			$txt=meldung(2790, true);	// 2790: Gesamtsumme nicht verrechenbarer Stunden
			lw_zeile('*gesamtsumme_l', '&nbsp;'.$txt, $csa['gesamtsumme_r'].get_entry($gnsek/3600.00, 'dezimal').'&nbsp;h&nbsp;', $csa['gesamtsumme_r'].get_entry(0, 'dezimal').'&nbsp;'.$currency.'&nbsp;');

			$lc=leerzeile(426, $mc, $lc, true);

			// Anteil
			$txt_ant=explode('|', meldung(2791, true));	// 2791: Anteil verrech. Stunden|Anteil nicht verrech. Stunden
			lw_zeile('~<b>'.$txt_ant[0].':</b>', $csa['right'].'<b>'.get_entry($gvsek*100.00/($gvsek+$gnsek), 'dezimal').'&nbsp;%</b>', '~<b>'.$txt_ant[1].':</b>', $csa['right'].'<b>'.get_entry($gnsek*100.00/($gvsek+$gnsek), 'dezimal').'&nbsp;%</b>');
		}
		else
			lw_leer();
	}

	// Projektkategorien
	$pcga=array();
	foreach($_POST as $pkey => $pvalue)
	{
		if((substr((string)$pkey, 0, 4) == 'pcg_') && $pvalue)
		{
			$pgid=(int)substr((string)$pkey, 4);
			$gunique_id=$pga[$pgid];
			$pcga[$pgid]=array('mode'=>(int)$pvalue, 'gid'=>$gunique_id, 'cid'=>array());

			foreach($pg[$gunique_id]['cat'] as $cunique_id => $cdat)
				$pcga[$pgid]['cid'][$cdat['id']]=$cunique_id;
		}
	}
//fxDebug($pcga, '$pcga');

	if(sizeof($pcga))
	{
		psb_zeitdaten($Personen_ID, $zvon, $zbis);
		$mpdat=array();

		$lit_pg=get_text(10, '20');		// Prj.-Kategoriegruppe
		$lit_oz=meldung(597, true);		// Ohne Zuordnung
		$lit_ma=get_text(627, '20');	// Mitarbeiter

		$tsum=0;
//fxDebug($zdat,'$zdat');
		foreach($pcga as $gid => $gdat)
		{
			$gunique_id=$gdat['gid'];
			$pd=array('sum'=>0, 'start'=>0, 'pos'=>$pg[$gunique_id]['pos'], 'col'=>$pg[$gunique_id]['col'], 'bg'=>$pg[$gunique_id]['bg'], 'val'=>$pg[$gunique_id]['val'], 'mpid'=>array(), 'cat'=>array());

			$lc=leerzeile(426, $mc, $lc, true);
			if($gid)
				lw_ueberschrift($lit_pg.' ['.$pd['pos'].' - '.$pd['val'].']');
			else
				lw_ueberschrift($lit_pg.' ['.$pd['val'].']');

			if(sizeof($gdat['cid']))
			{
				foreach($gdat['cid'] as $cid => $cunique_id)
					$pd['cat'][$cid]=array('sum'=>0, 'pos'=>$pg[$gunique_id]['cat'][$cunique_id]['pos'], 'col'=>$pg[$gunique_id]['cat'][$cunique_id]['col'], 'bg'=>$pg[$gunique_id]['cat'][$cunique_id]['bg'], 'val'=>$pg[$gunique_id]['cat'][$cunique_id]['val'], 'mpid'=>array());
			}

			if(sizeof($zdat))
			{
				foreach($zdat as $z)
				{
					$cid=(int)$z['kategorie_id'];
					if(($gid == $cid) || isset($gdat['cid'][$cid]))
					{
						$pid=$z['projekt_id'];
						if($pid >= 0)
						{
							$sek=$z['zeitaufwand'];
							$mpid=$z['mutterprojekt_id'];
//echo('found '.$cid.': mpid='.$mpid.', sek='.$sek.'<br>');

							$pd['sum'] += $sek;
							if($gid == $cid)
								$pd['start'] += $sek;
							else
								$pd['cat'][$cid]['sum'] += $sek;

							if(!isset($mpdat[$mpid]))
							{
								if($mpid > 0)
								{
									$sql="SELECT vorgangsnummer, name_projekt FROM projekte WHERE projekt_id=".$mpid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
									$tmp=db_values($sql);
//fxDebug($tmp, $sql);
								}
								else
									$tmp=array('vorgangsnummer'=>0, 'name_projekt'=>"[".meldung(681, false)."]");		// Unbekannt

								if(!fxIsArray($tmp))
									$mpdat[$mpid]=array('srt'=>"???", 'nam'=>"<i>???</i>");
								else
								{
									if($mpid > 0)
										$nam='<font class="blue">'.$tmp['name_projekt'].'</font>';
									else
										$nam='<font class="red">'.$tmp['name_projekt'].'</font>';

									if(strtolower((string)$GLOBALS['fxpglobals']['settings']['zeit_projektnr']) == 'ja')
										$mpdat[$mpid]=array('srt'=>$tmp['vorgangsnummer'], 'nam'=>no_null($tmp['vorgangsnummer']).': '.$nam);
									else
										$mpdat[$mpid]=array('srt'=>strtolower((string)$tmp['name_projekt']).$tmp['vorgangsnummer'], 'nam'=>$nam.' ('.no_null($tmp['vorgangsnummer']).')');
								}
							}
							$srt=$mpdat[$mpid]['srt'];

							if($gid == $cid)
							{
								if(!isset($pd['mpid'][$srt]))
									$pd['mpid'][$srt]=array('id'=>$mpid, 'sum'=>$sek);
								else
									$pd['mpid'][$srt]['sum'] += $sek;
							}
							else if(!isset($pd['cat'][$cid]['mpid'][$srt]))
								$pd['cat'][$cid]['mpid'][$srt]=array('id'=>$mpid, 'sum'=>$sek);
							else
								$pd['cat'][$cid]['mpid'][$srt]['sum'] += $sek;
						}
					}
				}
			}
			if(sizeof($gdat['cid']))
			{
				foreach($gdat['cid'] as $cid => $cunique_id)
				{
					if(!sizeof($pd['cat'][$cid]['mpid']))
						$pd['cat'][$cid]['mpid']['???']=array('id'=>0, 'sum'=>0);
				}
			}
//fxDebug($pd, '$pd');

			// Projektkategorie(n)
			if($gdat['mode'] > 1)
			{
				$ccnt=0;
				if($pd['start'] > 0)
				{
					if(($gdat['mode'] > 2) && $ccnt)
						$lc=leerzeile(426, $mc, $lc, true);

					$txt='&nbsp;&nbsp;&nbsp;';
					$bg='';
					if(strlen((string)$pd['col']) && ($pd['col'] > 0))
						$bg='<span style="border-radius:4px;background:'.$pd['bg'].';">&nbsp;&nbsp;</span>&nbsp;';
					$txt .= $bg.$pd['pos'].'&nbsp;['.$pd['val'].']';
					if($gdat['mode'] > 2)
						$pcsum='~|';
					else
						$pcsum=$csa['right'].get_entry($pd['start']/3600.00, 'dezimal').'&nbsp;h';
					lw_zeile('^col_?" nowrap colspan="4"><!--|~'.$txt, '~', '~-->'.$txt, $pcsum);

					// Mutterprojekt(e)
					if($gdat['mode'] > 2)
					{
						ksort($pd['mpid'],SORT_STRING);
						foreach($pd['mpid'] as $mdat)
						{
							$mpid=$mdat['id'];
							if(isset($mpdat[$mpid]))
								$mnam=$mpdat[$mpid]['nam'];
							else
								$mnam='<i class="grey">('.$lit_oz.')</i>';
							$txt='&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'pt59.png" align="top">&nbsp;'.$mnam;
							lw_zeile('^col_?" nowrap colspan="4"><!--|~'.$txt, '~', '~-->'.$txt, $csa['right'].get_entry($mdat['sum']/3600.00, 'dezimal').'&nbsp;h');
						}

						// Startsumme
						$txt='&nbsp;&nbsp;&nbsp;'.$lit_startsumme.' ['.$pd['val'].']';
						lw_zeile(str_replace('|~','" colspan="4"><!--|~',$csa['zwischensumme_l']).'&nbsp;'.$txt, '~', '~-->'.$txt, $csa['zwischensumme_r'].get_entry($pd['start']/3600.00, 'dezimal').'&nbsp;h&nbsp;');
					}

					$ccnt++;
				}

				foreach($pd['cat'] as $cid => $cdat)
				{
//fxDebug($cdat,'$cdat');
					if(($gdat['mode'] > 2) && $ccnt)
						$lc=leerzeile(426, $mc, $lc, true);

					$txt='&nbsp;&nbsp;&nbsp;&middot;&nbsp;';
					if($cid)
					{
						$bg='';
						if(strlen((string)$cdat['col']) && ($cdat['col'] > 0))
							$bg='<span style="border-radius:4px;background:'.$cdat['bg'].';">&nbsp;&nbsp;</span>&nbsp;';
						$txt .= $bg.$cdat['pos'].'&nbsp;['.$cdat['val'].']';
					}
					else
						$txt .= '<i>['.$cdat['val'].']</i>';
					if($gdat['mode'] > 2)
						$pcsum='~|';
					else
						$pcsum=$csa['right'].get_entry($cdat['sum']/3600.00, 'dezimal').'&nbsp;h';
					lw_zeile('^col_?" nowrap colspan="4"><!--|~'.$txt, '~', '~-->'.$txt, $pcsum);

					// Mutterprojekt(e)
					if($gdat['mode'] > 2)
					{
//fxDebug($cdat['mpid'],'$cdat[mpid]');
						ksort($cdat['mpid'],SORT_STRING);
						foreach($cdat['mpid'] as $mdat)
						{
							$mpid=$mdat['id'];
							if(isset($mpdat[$mpid]))
								$mnam=$mpdat[$mpid]['nam'];
							else
								$mnam='<i class="grey">('.$lit_oz.')</i>';
							$txt='&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'pt59.png" align="top">&nbsp;'.$mnam;
							lw_zeile('^col_?" nowrap colspan="4"><!--|~'.$txt, '~', '~-->'.$txt, $csa['right'].get_entry($mdat['sum']/3600.00, 'dezimal').'&nbsp;h');
						}

						// Zwischensumme
						$txt='&nbsp;&nbsp;&nbsp;'.$lit_zwischensumme.' ['.$cdat['val'].']';
						lw_zeile(str_replace('|~','" colspan="4"><!--|~',$csa['zwischensumme_l']).'&nbsp;'.$txt, '~', '~-->'.$txt, $csa['zwischensumme_r'].get_entry($cdat['sum']/3600.00, 'dezimal').'&nbsp;h&nbsp;');
					}

					$ccnt++;
				}
			}
			// Gesamtsumme
			$tsum += $pd['sum'];
			$txt=$lit_gesamtsumme.' ['.$pd['val'].']';
			lw_zeile(str_replace('|~','" colspan="4"><!--|~',$csa['gesamtsumme_l']).'&nbsp;'.$txt, '~', '~-->'.$txt, $csa['gesamtsumme_r'].get_entry($pd['sum']/3600.00, 'dezimal').'&nbsp;h&nbsp;');
		}

		$lc=leerzeile(426, $mc, $lc, true);
		lw_ueberschrift($lit_gesamtsumme.' '.$lit_pg);
		$txt=$lit_gesamtsumme.' '.$lit_ma;
		lw_zeile(str_replace('|~','" colspan="4"><!--|~',$csa['gesamtsumme_l']).'&nbsp;'.$txt, '~', '~-->'.$txt, $csa['gesamtsumme_r'].get_entry($tsum/3600.00, 'dezimal').'&nbsp;h&nbsp;');
	}
}
if(!fxIsArray($GLOBALS['fxptdata']['lvalues'][$mc]))
	$GLOBALS['fxptdata']['lvalues'][$mc]=true;
else
	drawPrint('mt426_rahmen', meldung(1669,true,$pi_name), 'S');


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * ???FUNCTION???
 *
 * @param various $txt - Mandatory parameter: ???PARAMETER???
 */
function lw_ueberschrift($txt)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$txt='&nbsp;'.$txt.'&nbsp;';
	$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text1']='^class_maskt" nowrap colspan="5"><!--|~'.$txt;
	$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text5']='~-->'.$txt;

	$GLOBALS['lc']++;
}

/**
 * ???FUNCTION???
 */
function lw_leer()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text1']='^col_?" colspan="5" align="right" nowrap style="padding:3px;"><!--|~'.$GLOBALS['txt_noentries'];
	$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text5']='~-->'.$GLOBALS['txt_noentries'];

	$GLOBALS['lc']++;
}

/**
 * ???FUNCTION???
 *
 * @param various $lit1 - Mandatory parameter:                 ???PARAMETER???
 * @param various $txt1 - Mandatory parameter:                 ???PARAMETER???
 * @param string  $lit2 - Optional parameter (default = ''):   ???PARAMETER???
 * @param string  $txt2 - Optional parameter (default = ''):   ???PARAMETER???
 * @param string  $dp   - Optional parameter (default = ':'):  ???PARAMETER???
 */
function lw_zeile($lit1, $txt1, $lit2='', $txt2='', $dp=':')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ucs='';
	if(!$lit1 || ($lit1 === NULL) || (substr((string)$lit1,0,1) == '*'))
	{
		if(($txt1 !== NULL) && $txt1)
		{
			if(($lit1 !== NULL) && substr((string)$lit1,0,1) == '*')
			{
				$ucs=$GLOBALS['csa'][substr((string)$lit1,1)];
				if(substr((string)$txt1,0,1) == '~')
					$txt1=substr((string)$txt1,1);
				$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text1']=str_replace('|~','" colspan="2"><!--|~',$ucs).$txt1;
				$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text2']='~-->'.$txt1;
			}
			else
			{
				$txt1='<span class="lit">'.$txt1.':</span>';
				$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text1']='^col_?" nowrap valign="top" colspan="2"><!--|~'.$txt1;
				$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text2']='~-->'.$txt1;
			}
		}
		else
		{
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text1']='~|';
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text2']='~|';
		}
	}
	else
	{
		// Literal
		if($lit1 == '|')
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text1']='~&nbsp;';
		else if(($lit1 !== NULL) && ((substr((string)$lit1, 0, 1) == '^') || (substr((string)$lit1, 0, 1) == '~')))
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text1']=$lit1;
		else
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text1']='~<span class="lit">'.$lit1.$dp.'</span>';
		// Text
		if(($txt1 !== NULL) && ((substr((string)$txt1, 0, 1) == '^') || (substr((string)$txt1, 0, 1) == '~')))
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text2']=$txt1;
		else
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text2']='~'.$txt1;
	}

	if(strlen((string)$ucs))
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text3']=$ucs;
	else
	{
		foreach($GLOBALS['csa'] as $cs)
		{
			if(($txt1 !== NULL) && (substr((string)$txt1,0,strlen((string)$cs)) == $cs))
			{
				$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text3']=$cs;
				break;
			}
		}
	}
//	$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text3']='~|';

	if(!$lit2)
	{
		if($txt2)
		{
			$txt2='<span class="lit">'.$txt2.':</span>';
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text4']='^col_?" nowrap valign="top" colspan="2"><!--|~'.$txt2;
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text5']='~-->'.$txt2;
		}
		else
		{
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text4']='~|';
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text5']='~|';
		}
	}
	else
	{
		// Literal
		if($lit2 == '|')
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text4']='~&nbsp;';
		else if(($lit2 !== NULL) && ((substr((string)$lit2, 0,1) == '^') || (substr((string)$lit2, 0,1) == '~')))
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text4']=$lit2;
		else
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text4']='~<span class="lit">'.$lit2.$dp.'</span>';
		// Text
		if(($txt2 !== NULL) && ((substr((string)$txt2, 0,1) == '^') || (substr((string)$txt2, 0,1) == '~')))
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text5']=$txt2;
		else
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['lc']]['nv_text5']='~'.$txt2;
	}

	$GLOBALS['lc']++;
}

/**
 * ???FUNCTION???
 *
 * @param various $pid - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function psb_adresse($pid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$pid)
		return;

	$sql="SELECT strasse, plz, ort, staat, landkz, adresstyp FROM adressen WHERE personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY adresstyp, landkz";
	$arr=db_values($sql, '*');

	return $arr;
}

/**
 * ???FUNCTION???
 *
 * @param various $pid - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function psb_kommunikation($pid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$pid)
		return;

	$sql="SELECT kommunikationstyp, kommunikation_txt, anmerkung FROM kommunikation WHERE personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY kommunikationstyp,kommunikation_txt";
	$arr=db_values($sql, '*');

	return $arr;
}

/**
 * ???FUNCTION???
 *
 * @param various $pid - Mandatory parameter: ???PARAMETER???
 * @param various $von - Mandatory parameter: ???PARAMETER???
 * @param various $bis - Mandatory parameter: ???PARAMETER???
 */
function psb_zeitdaten($pid, $von, $bis)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(isset($GLOBALS['zdat']))
		return;

	$GLOBALS['zdat']=array();

	if(!$pid)
		return;

	$von=trim((string)$von);
	if(!strlen((string)$von))
		$von="'19000101000000'";
	else
		$von="'".substr((string)$von, 0, 8)."000000'";

	$bis=trim((string)$bis);
	if(!strlen((string)$bis))
		$bis="'99991231240000'";
	else
		$bis="'".substr((string)$bis, 0, 8)."240000'";

	$sql  = "SELECT";
	$sql .= " z.projekt_id, z.ist_beginn, z.ist_ende, z.zeitaufwand, z.pause, z.kosten_ek, z.kosten_vk, NULL AS stdek, NULL AS stdvk, z.nicht_abrechenbar, z.zeitart,";
	$sql .= " p.mutterprojekt_id, p.kategorie_id ";
	$sql .= "FROM";
	$sql .= " zeitdaten z, projekte p ";
	$sql .= "WHERE";
	$sql .= " z.personen_id=".$pid." AND z.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND z.zeitaufwand>0 AND z.zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND (z.ist_beginn<=".$bis." AND z.ist_ende>=".$von.")";
	$sql .= " AND p.projekt_id=z.projekt_id AND p.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].") ";
	$sql .= "ORDER BY";
	$sql .= " ist_beginn";
	$arr=db_values($sql, '*');
//fxDebug($arr, $sql);

	if(fxIsArray($arr))
		$GLOBALS['zdat']=$arr;
}
?>