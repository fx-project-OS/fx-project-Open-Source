<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : calendar.inc                                                 //
// Version     : 21.2                                                         //
// Begin       : 2020-11-06                                                   //
// Last Change : 2020-11-06                                                   //
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
 * Function collection for creation the calendar table
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;

$caption_strtr = array('\''=>"&apos;", '"'=>"&quot;", '<'=>"&lt;", '>'=>"&gt;");


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * Create the calendar table
 *
 * @param various $k_headline      - Mandatory parameter:                ???PARAMETER???
 * @param various $k_inhalt1       - Mandatory parameter:                ???PARAMETER???
 * @param various $k_inhalt2       - Mandatory parameter:                ???PARAMETER???
 * @param string  $k_headline_text - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $width           - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $k_help          - Optional parameter (default = ''):  ???PARAMETER???
 * @param integer $fixcols         - Optional parameter (default = 0):   ???PARAMETER???
 */
function kalender($k_headline, $k_inhalt1, $k_inhalt2, $k_headline_text='', $width='', $k_help='', $fixcols=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $nl;

	// Kalender darstellen
	if($k_headline_text)
		$headline = $k_headline_text;
	else
		$headline = meldung($k_headline);
	if($k_help)
		$k_help = '|'.$k_help;

	$ntxt = '';
	// Fehlermeldung, falls angegeben (String beginnt mit ?)
	if(substr($k_inhalt1, 0, 1) == '?')
	{
		$ntxt = '<font class="red">'.substr($k_inhalt1, 1).'</font>'.$nl;
		$fehlermeldung = true;
	}

	// Inhalt darstellen, falls angegeben
	else if($k_inhalt1)
		$ntxt .= $k_inhalt1;

	if($k_inhalt2)
		$ntxt .= $k_inhalt2;

	// ansonsten Einträge darstellen
	else if(fxIsArray($GLOBALS['fxptdata']['cal']) && !$fehlermeldung)
	{
		$ntxt .= '<div id="cal_frame" style="position:relative;margin-bottom:4px;max-width:'.($GLOBALS['fxpglobals']['swidth']-100).'px;overflow-x:auto;overflow-y:hidden;" onscroll="$(\'cal_ffr\').style.left=$(\'cal_frame\').scrollLeft+\'px\';">'.$nl;

		// Infospalten fest
		if(!$fixcols && isset($GLOBALS['fxptdata']['cal'][0][0]['fixcols']))
			$fixcols=(int)$GLOBALS['fxptdata']['cal'][0][0]['fixcols'];
		if(!$fixcols && fxIsArray($GLOBALS['fxptdata']['cal'][2]))
		{
			foreach($GLOBALS['fxptdata']['cal'][2] as $c => $la)
			{
				if(isset($la['text']) && (substr($la['text'],0,4) === '<div'))
				{
					$fixcols=$c;
					break;
				}
			}
		}
//echo('$fixcols='.$fixcols.'<hr />');

		if($fixcols > 0)
		{
			$ntxt .= '<div id="cal_ffr" style="position:absolute;left:0;top:0;z-index:128;box-shadow:0 0 8px #444444;">'.$nl;
			$ntxt .= '<table width="1" border="0" cellpadding="0" cellspacing="1" bgcolor="#cccccc">'.$nl;
			// Alle Zeilen durchgehen
			for($c=0; $c<sizeof($GLOBALS['fxptdata']['cal']); $c++)
			{
				$id = ' id="tr'.$c.'"';

				$height = 32;
				if($GLOBALS['fxptdata']['cal'][$c][0]['height'])
					$height = $GLOBALS['fxptdata']['cal'][$c][0]['height'];
				else if(($c < 3) || ($GLOBALS['fxpglobals']['tr'] != 26))
					$height = 19;

				$disp = '';
				if(isset($GLOBALS['fxptdata']['cal'][$c][0]['disp']))
					$disp = $GLOBALS['fxptdata']['cal'][$c][0]['disp'];
				$style = ' style="display:'.$disp.';"';

				$ntxt .= '<tr'.$id.' height="'.$height.'"'.$style.'>'.$nl;

				$spalte=0;
				if(!fxIsArray($GLOBALS['fxptdata']['cal'][$c][$spalte]))
					$spalte=key($GLOBALS['fxptdata']['cal'][$c]);
				while(fxIsArray($GLOBALS['fxptdata']['cal'][$c][$spalte]) && ($spalte < $fixcols))
				{
					$ntxt .= kalenderspalten($GLOBALS['fxptdata']['cal'][$c][$spalte], $c, $spalte, $height);
					if($GLOBALS['fxptdata']['cal'][$c][$spalte]['colspan'])
						$spalte += $GLOBALS['fxptdata']['cal'][$c][$spalte]['colspan'];
					else
						$spalte++;
				}
				$ntxt .= '</tr>'.$nl;
			}
			$ntxt .= '</table>'.$nl;
			$ntxt .= '</div>'.$nl;
		}

		// Tabelle zeichnen
		$ntxt .= '<table width="1" border="0" cellpadding="0" cellspacing="1" bgcolor="#cccccc">'.$nl;
		// Alle Zeilen durchgehen
		for($c=0; $c<sizeof($GLOBALS['fxptdata']['cal']); $c++)
		{
			$id = ' id="tr'.$c.'"';

			$height = 32;
			if($GLOBALS['fxptdata']['cal'][$c][0]['height'])
				$height = $GLOBALS['fxptdata']['cal'][$c][0]['height'];
			else if(($c < 3) || ($GLOBALS['fxpglobals']['tr'] != 26))
				$height = 19;

			$disp = '';
			if(isset($GLOBALS['fxptdata']['cal'][$c][0]['disp']))
				$disp = $GLOBALS['fxptdata']['cal'][$c][0]['disp'];
			$style = ' style="display:'.$disp.';"';

			$ntxt .= '<tr'.$id.' height="'.$height.'"'.$style.'>'.$nl;

			$spalte=0;
			if(!fxIsArray($GLOBALS['fxptdata']['cal'][$c][$spalte]))
				$spalte=key($GLOBALS['fxptdata']['cal'][$c]);
			while(fxIsArray($GLOBALS['fxptdata']['cal'][$c][$spalte]))
			{
				$ntxt .= kalenderspalten($GLOBALS['fxptdata']['cal'][$c][$spalte], $c, $spalte, $height);
				if(isset($GLOBALS['fxptdata']['cal'][$c][$spalte]['ausl']))
					$GLOBALS['fxptdata']['cal'][$c][$spalte]['text'] = $GLOBALS['fxptdata']['cal'][$c][$spalte]['ausl'];
				if($GLOBALS['fxptdata']['cal'][$c][$spalte]['colspan'])
					$spalte += $GLOBALS['fxptdata']['cal'][$c][$spalte]['colspan'];
				else
					$spalte++;
			}
			$ntxt .= '</tr>'.$nl;
		}
		$ntxt .= '</table>'.$nl;

		$ntxt .= '</div>';

		if(!$GLOBALS['hide_dm'])
		{
			// Datei-Manager
			$dm_arg='&tr='.$GLOBALS['fxpglobals']['tr'].'&headline='.urlencode($headline);
			$GLOBALS['fxpvars']['ibuffer'][]=fieldLinkImage('icon_druck','im_dm','loader.php?url=d_manager_pst.inc'.$dm_arg,'<b>'.meldung(1578).'</b>: '.$headline);
		}
	}

	msk_window($headline, $ntxt, true);
}

/**
 * Create the calendar columns
 *
 * @param various $spalte    - Mandatory parameter:                ???PARAMETER???
 * @param various $aktzeile  - Mandatory parameter:                ???PARAMETER???
 * @param various $aktspalte - Mandatory parameter:                ???PARAMETER???
 * @param integer $height    - Optional parameter (default = 32):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function kalenderspalten($spalte, $aktzeile, $aktspalte, $height=32)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $nl;

	extract($spalte);

	$trans_table = array(chr(9)=>'&nbsp;', chr(10).chr(10)=>', ', chr(10)=>', ');
	$rtxt = '';

	$cls='';
	if(isset($class))
	{
		$cls=trim($class);
		if(strlen($cls))
			$cls=' class="'.$cls.'"';
	}
	$col='';
	if(isset($color))
	{
		$color=trim($color);
		if(strlen($color))
			$col=' bgcolor="#'.$color.'"';
	}

	$dtr=$GLOBALS['fxpglobals']['tr'];
	if(strlen($link))
	{
		$link=str_replace('"', '', $link);
		$llink=strtolower($link);
		$tpos=strpos($llink, '&tr=');
		if($tpos !== false)
			$dtr=(int)substr($link, $tpos+4);
		else
		{
			$tpos=strpos($llink, '?tr=');
			if($tpos !== false)
				$dtr=(int)substr($link, $tpos+4);
		}
	}

	// Leere Einträge schnell abarbeiten
	if(strlen($text) == 0)
	{
		$s='';
		if(strlen($style))
			$s=$style;
		$t='&nbsp;';
		if(strlen($itext))
			$t=$itext;
		$a='';
		if($align)
			$a=' align="'.$align.'"';
		if($tooltip || $tooltip_js)
		{
			if(!$tooltip_js)
				$tooltip_js = $tooltip;
			$s .= 'cursor:help;';
			$tooltip=' tooltip="'.$tooltip_js.'"';
		}
		if($tooltip_ic)
			$t='<div'.$cls.' style="position:relative;">'.$t.'<img src="'.$GLOBALS['gfxpath'].'tts.png" style="position:absolute;top:-1px;right:1px;"></div>';
		if(strlen($s))
			$s=' style="'.$s.'"';
		if($colspan)
		{
			if($typ == 'nv')
			{
				$agr = 'au100';
				if($GLOBALS['fxpglobals']['tr'] == 92)	// Urlaubsplan
					$agr = 'pr999';
				$rtxt .= str_repeat('<td'.$cls.$a.' background="'.$GLOBALS['gfxpath'].$agr.'.png"'.$s.$tooltip.'>'.$t.'</td>'.$nl, $colspan);
			}
			else if($typ == 'background')
				$rtxt .= str_repeat('<td'.$cls.$a.' background="'.$GLOBALS['gfxpath'].strtolower($background).'.png"'.$s.$tooltip.'>'.$t.'</td>'.$nl, $colspan);
			else
			{
				if($GLOBALS['fxpglobals']['tr'] == 92)	// Urlaubsplan
					$col = ' bgcolor="#e5e5ef"';
				$rtxt .= str_repeat('<td'.$cls.$a.$col.$s.$tooltip.'>'.$t.'</td>'.$nl, $colspan);
			}
		}
		else if(is_array($background))
		{
			foreach($background as $c)
				$rtxt .= '<td'.$cls.$a.' background="'.$GLOBALS['gfxpath'].strtolower($c).'.png"'.$s.$tooltip.'>'.$t.'</td>';
			$rtxt .= $nl;
		}
		return $rtxt;
	}

	$text_origin = $text;
	while(true)
	{
		$text = $text_origin;
		if((!$aktzeile || (($aktzeile == 1) && !isset($GLOBALS['fxptdata']['cal'][0][$aktspalte]))) && ($aktspalte > $GLOBALS['xoffset']))
		{
			if(!$aktzeile)
				$bc='000000';
			else
				$bc='888888';
			$tdi='<div style="position:absolute;top:'.($aktzeile*$height + 1).'px;height:100%;border-left:1px solid #'.$bc.';">&nbsp;</div>';
		}
		else
			$tdi='';
		// <td> - Definition setzen
		$td = '<td'.$cls.$col.' nowrap';
		if($colspan > 1)
			$td .= ' colspan="'.min($colspan, 1000).'"';
		if($rowspan > 1)
			$td .= ' rowspan="'.$rowspan.'"';
		if(!$aktzeile && !$aktspalte)
			$td .= ' width="100%"';
		else if($width)
			$td .= ' width="'.$width.'"';
		if($align)
			$td .= ' align="'.$align.'"';
		if($background)
		{
			if(substr($background,0,1) == '#')
				$td .= ' bgcolor="'.$background.'"';
			else
				$td .= ' background="'.$GLOBALS['gfxpath'].strtolower($background).'.png"';
		}
		$tdstyle = '';
		$tdaction = '';
		if(strlen($style))
			$tdstyle=$style;
		if(strlen($padding))
			$tdstyle .= 'padding:'.$padding.';';
		if($ts)
			$tdstyle .= 'box-shadow:-6px -2px 4px #a90013 inset;border-right:1px solid #a90013;';
		if($aktzeile == 2)
			$tdstyle .= 'border-bottom:1px solid #444444;';
		if($tooltip || $tooltip_js)
		{
			if(!$tooltip_js)
				$tooltip_js = $tooltip;
			$tdstyle .= 'cursor:help;';
			$tdaction .= ' tooltip="'.$tooltip_js.'"';
		}
		if(strlen($link) && $link)
		{
			$tdstyle .= 'cursor:pointer;';
			$tdaction .= ' '.fxf_jsFunction('loadTR', '>1', '^loading', $dtr, $link.'&keep_get=1');
		}
		if(strlen($tdstyle))
			$td .= ' style="text-decoration:none;'.$tdstyle.'"';
		if(strlen($tdaction))
			$td .= $tdaction;
		$td .= '>';

		// <td> - Inhalt anzeigen
		// Text
		if($typ != 'All')
		{
			$c=$colspan;
			if(!$c)
				$c=1;
			$w=(int)$GLOBALS['fxptdata']['cal'][2][$aktspalte]['width'];
			if(!$w)
				$w=22;
			$cw=$c*$w;
			$cs=round($cw/9);
//echo($aktzeile.'-'.$aktspalte.': $text='.$text.', $w='.$w.', $c='.$c.', $cw='.$cw.' -- $cs='.$cs.'<br>');
			$text_backup = $text;
			if($text == '~')
				$text = '&nbsp;';
			else if($text == '^')
				$text = '';
			else if(substr($text, 0, 1) == '~')
				$text = substr($text, 1);
			else
			{
				$tchars = array();
				$tc = 0;
				while($tc<strlen($text))
				{
					if((substr($text, $tc, 2) == '&#') || (substr($text,$tc,6) == '&nbsp;'))	// Sonderzeichen
					{
						$skp = strpos($text, ';', $tc);
						if($skp > $tc)
						{
							$tchars[] = substr($text, $tc, 1+$skp-$tc);
							$tc = $skp+1;
						}
						else
						{
							$tchars[] = substr($text, $tc, 1);
							$tc++;
						}
					}
					else
					{
						$tchars[] = substr($text, $tc, 1);
						$tc++;
					}
				}
//fxDebug($tchars, '$tchars');
				if(fxIsArray($tchars))
				{
					$text = '';
					for($tc=0; $tc<$cs; $tc++)
					{
						if(isset($tchars[$tc]))
							$text .= $tchars[$tc];
					}
				}
				else
					$text = substr($text, 0, $cs);
				$text = strtr($text, $GLOBALS['caption_strtr']);
			}
		}
		if($rtext)
			$tdi .= $rtext;
		if($ltext)
			$tdi .= $ltext;
		if($ltextz)
			$tdi .= $ltextz;
		if(strlen($text) > 0)
		{
			if($font)
				$tdi .= '<font '.$font.'>';
			if(substr($text,0,1) == ' ')
				$tdi .= '&nbsp;'.substr($text,1);
			else
				$tdi .= $text;
			if($font)
				$tdi .= '</font>';
		}
		if($rtextz)
			$tdi .= $rtextz;

		// Link
		if($link)
		{
			$td .= '<a';
			if($tooltip || $tooltip_js)
			{
				if(!$tooltip_js)
					$tooltip_js = $tooltip;
				$td .= ' tooltip="'.$tooltip_js.'"';
			}
			if($anchor)
				$td .= ' name="pj'.($aktzeile-2).'"';
			$td .= '>';
			$tdi .= '</a>';
		}

		// <td> schliessen und anzeigen
		$rtxt .= $td.$tdi.'</td>'.$nl;
		if($colspan > 1000)
			$colspan -= 1000;
		else
			break;
	}
//echo('<pre>'.fxHtmlEncode($rtxt).'</pre>');
	return $rtxt;
}

/**
 * Create a calendar line
 *
 * @param various $persid    - Mandatory parameter:                 ???PARAMETER???
 * @param various &$aktzeile - Mandatory parameter:                 ???PARAMETER???
 * @param various $ka_debug  - Mandatory parameter:                 ???PARAMETER???
 * @param various $ka_string - Mandatory parameter:                 ???PARAMETER???
 * @param string  $einheit   - Optional parameter (default = 'D'):  ???PARAMETER???
 */
function kalenderzeile($persid, &$aktzeile, $ka_debug, $ka_string, $einheit='D')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $smand, $pr_gesamtaufwand, $pb_string;

	$mode = 1;
	if($_POST['nur_plandaten'])
		$mode = 0;

	$mid = $GLOBALS['fxpglobals']['client'];
	if(fxIsArray($GLOBALS['personen']) && fxIsArray($GLOBALS['personen'][$persid]) && isset($GLOBALS['personen'][$persid]['mandanten_id']))
		$mid = (int)$GLOBALS['personen'][$persid]['mandanten_id'];

	// Person darstellen
	if($ka_debug && $ka_debug <= 3)
		echo('<U><b>Person:</b> '.$GLOBALS['personen'][$persid]['fullname'].'</U><BR>');
	$p_timeline='';
	$pz=0;
	if(fxIsArray($GLOBALS['personen'][$persid]))
	{
		$today=substr($GLOBALS['datetime'],0,8);

		$von_disp	= substr($GLOBALS['von'], 0, 8);
		$bis_disp	= substr($GLOBALS['bis'], 0, 8);
		$von_min	= $von_disp;
		$bis_max	= $bis_disp;
//echo('$von_disp='.$von_disp.', $bis_disp='.$bis_disp.'<br>');

		// Mitarbeiter nur anzeigen, oder auch änderbar?
		$disabled=false;
		$disabled_name=false;
		if(!$GLOBALS['personen'][$persid]['aktiv'])
			$disabled_name=true;

		if($_POST['ZAuslastung'] || $_POST['ZProjekte'])
		{
			if(is_array($GLOBALS['personen'][$persid]['projekte']))
			{
				foreach($GLOBALS['personen'][$persid]['projekte'] as $pkey => $value)
				{
					$von_min = min($von_min, substr($value['soll_beginn'], 0, 8));
					$bis_max = max($bis_max, substr($value['ber_ende'], 0, 8));

					if($_POST['ZProjekte'] && $value['disp'])
						$pz++;
				}
				// Falls Person zu Projekten zugeordnet ist, jedoch keine im Zeitraum liegen
				if(!$pz && $_POST['ZProjekte'])
					$pz = 1;
			}
			else if($_POST['ZProjekte'])
				$pz = 1;
		}
//echo('$von_min='.$von_min.', $bis_max='.$bis_max.'<br>');

		// Arbeitstage, Feiertage, Sondertage + Termine
		$pers_array = getExtPersInfo($persid, $von_min, $bis_max);
		$term_array	= $pers_array['days'];
//fxDebug($pers_array, 'getExtPersInfo('.$persid.', '.$von_min.', '.$bis_max.');');
//fxDebug($term_array, '$term_array');

		if(!isset($GLOBALS['delta_maus']))
		{
			$GLOBALS['delta_maus'] = $GLOBALS['personen'][$persid]['maus'];
			$GLOBALS['delta_maus']['cal'] = array('text'=>"", 'align'=>"right", 'color'=>"ffffff", 'typ'=>"All", 'ausl'=>"", 'tooltip_js'=>"");
		}
		else
			$GLOBALS['delta_maus']['sstd'] += $GLOBALS['personen'][$persid]['maus']['sstd'];

		$ausl_array	= array();
		if($_POST['ZAuslastung'])
		{
			$pz++;
			if(($einheit == 'A') || ($einheit == 'D'))
				$pz++;

			if(fxIsArray($term_array))
			{
				$cs_off = 0;
				foreach($term_array as $tdate => $tinfo)
				{
					if(($tdate < $von_disp) || ($tdate > $bis_disp) || !isset($GLOBALS['oarr'][$tdate]))
						continue;

					$off = $GLOBALS['xoffset']+$GLOBALS['oarr'][$tdate];

					// Auslastung
					if(!isset($ausl_array[$off]))
					{
						$ausl_array[$off] = array('typ'=>"x", 'datum'=>$tdate, 'sstd'=>0.00, 'astd_ges'=>0.00, 'astd_term'=>0.00, 'astd_akt'=>0.00, 'astd_gen'=>0.00, 'astd_gepl'=>0.00);
						if($GLOBALS['darr'][$GLOBALS['oarr'][$tdate]]['type'] != 'day')
						{
							$ausl_array[$off]['typ'] = "c";
							if($cs_off)
								$ausl_array[$cs_off]['colspan'] = $off+1-$cs_off;
							else
							{
								$cs_off = $off;
								$ausl_array[$cs_off]['colspan'] = 1;
							}
						}
						else
							$cs_off = 0;
					}

					$zsstd = $GLOBALS['personen'][$persid]['maus']['sstd'];	// Sollstunden pro Tag
					$ztstd = 0.00;											// Terminstunden pro Tag

					// ...Kein Arbeitstag
					if(!$tinfo['wd'])
					{
						$zsstd = 0.00;

						if($ausl_array[$off]['typ'] == "x")
							$ausl_array[$off]['typ'] = 'w';
					}
					// ...Feiertag
					if(is_array($tinfo['hd']))
					{
						$diff   = $GLOBALS['personen'][$persid]['maus']['sstd']*$tinfo['hd']['typ'];
						$zsstd -= $diff;

						if($ausl_array[$off]['typ'] == "x")
							$ausl_array[$off]['typ'] = 'h';
					}
					// ...Sondertag
					if(is_array($tinfo['sd']))
					{
						$diff   = $tinfo['sd'][-1]['stunden_termin'];
						$zsstd -= $diff;

						if($ausl_array[$off]['typ'] == "x")
							$ausl_array[$off]['typ'] = 's'.$tinfo['sd'][-1]['typ'];
					}
					// ...Termin
					if(is_array($tinfo['ap']))
					{
						$diff   = $tinfo['ap'][-1]['stunden_termin'];
						$ztstd += $diff;

						if($ausl_array[$off]['typ'] == "x")
							$ausl_array[$off]['typ'] = 'a';
					}
					$zsstd = max($zsstd, 0.00);
					$ztstd = min($ztstd, $GLOBALS['personen'][$persid]['maus']['sstd']);

					$ausl_array[$off]['sstd']		+= $zsstd;
					$ausl_array[$off]['astd_term']	+= $ztstd;

					if($tdate >= $today)
						$ausl_array[$off]['astd_ges'] += $ztstd;

					$GLOBALS['personen'][$persid]['maus']['gstd']		+= $zsstd;
					$GLOBALS['personen'][$persid]['maus']['astd_term']	+= $ztstd;
//echo('&sum; '.$tdate.': $gstd='.$GLOBALS['personen'][$persid]['maus']['gstd'].' h (+ '.$zsstd.' h)<br>');
//echo('# '.$tdate.': $astd_term='.$GLOBALS['personen'][$persid]['maus']['astd_term'].' h (+ '.$ztstd.' h)<br>');

					$GLOBALS['delta_maus']['gstd']						+= $zsstd;
					$GLOBALS['delta_maus']['astd_term']					+= $ztstd;
				}

				// Ist-Stunden der Vergangenheit
				if($mode > 0)
				{
					// ...für Auslastung in der Vergangenheit
					if(sizeof($ausl_array) && sizeof($GLOBALS['darr']) && ($GLOBALS['darr'][0]['info']['mindate'] < $today))
					{
						$isd  = $GLOBALS['darr'][0]['info']['mindate'].'000000';
						$ied  = min($GLOBALS['darr'][0]['info']['maxdate'].'240001', $today.'000000');
						$sql  = "SELECT zeitaufwand, ist_beginn";
						$sql .= " FROM zeitdaten";
						$sql .= " WHERE personen_id=".$persid." AND mandanten_id=".$mid." AND projekt_id>=0";
						$sql .= " AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND zeitaufwand>0 AND ist_beginn>='".$isd."' AND ist_beginn<'".$ied."'";
						$tmp  = db_values($sql, '*');
//fxDebug($tmp, $sql);
						if(fxIsArray($tmp))
						{
							foreach($tmp as $t)
							{
								$idate = substr($t['ist_beginn'], 0, 8);
								if(isset($GLOBALS['oarr'][$idate]))
								{
									$off = $GLOBALS['xoffset']+$GLOBALS['oarr'][$idate];
									$std = round($t['zeitaufwand']/3600, 4);
									$ausl_array[$off]['astd_ges'] += $std;
									$GLOBALS['personen'][$persid]['maus']['astd_ist']	+= $std;
									$GLOBALS['delta_maus']['astd_ist']					+= $std;
								}
							}
						}
					}

					// ...für Projektauslastung
					if(fxIsArray($GLOBALS['personen'][$persid]['projekte']) && sizeof($GLOBALS['personen'][$persid]['projekte']))
					{
						$acsv = '';
						foreach($GLOBALS['personen'][$persid]['projekte'] as $p)
						{
							if(strlen($acsv))
								$acsv .= ',';
							$acsv .= $p['projekt_id'];
						}
						$sql  = "SELECT projekt_id, SUM(zeitaufwand) AS ist_sek";
						$sql .= " FROM zeitdaten";
						$sql .= " WHERE personen_id=".$persid." AND mandanten_id=".$mid." AND projekt_id IN (".$acsv.")";
						$sql .= " AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND zeitaufwand>0 AND ist_beginn<'".$today."000000'";
						$sql .= " GROUP BY projekt_id";
						$tmp  = db_values($sql, '*');
//fxDebug($tmp, $sql);
						if(fxIsArray($tmp))
						{
							foreach($tmp as $t)
								$GLOBALS['personen'][$persid]['projekte'][(int)$t['projekt_id']]['ist_sek'] = (int)$t['ist_sek'];
						}

						// ...Geschätzten Fertigstellungsgrad
						$sql  = "SELECT projekt_id, aufwand_soll, aufwand_ist, pk_fgrad FROM projektfortschritt";
						$sql .= " WHERE personen_id=".$persid." AND mandanten_id=".$mid." AND projekt_id IN (".$acsv.") AND aktiv_kz=1";
						$tmp = db_values($sql, '*');
//fxDebug($tmp, $sql);
						if(fxIsArray($tmp))
						{
							foreach($tmp as $t)
							{
								if($t['pk_fgrad'] > 0.00)
									$GLOBALS['personen'][$persid]['projekte'][(int)$t['projekt_id']]['fgrad'] = (double)($t['aufwand_ist']/$t['pk_fgrad']);
							}
						}
					}
				}
			}
		}
//fxDebug($ausl_array, '$ausl_array (VORHER): '.__LINE__);

		// Mitarbeiter-Name
		$tooltip_js = '';
		if($GLOBALS['fxpglobals']['tr'] != 14)	// nicht im Kontrollzentrum
		{
			$tooltip_js = '<table border=0 cellpadding=0 cellspacing=0>';
			// Eintrittsdatum/Austrittsdatum
			$einaus = get_entry($GLOBALS['personen'][$persid]['eintritt'], 'datum');
			if($GLOBALS['personen'][$persid]['austritt'])
				$einaus .= '&nbsp;-&nbsp;'.get_entry($GLOBALS['personen'][$persid]['austritt'], 'datum');
			$tooltip_js .= '<tr><td colspan=2 nowrap>'.$einaus.'</td></tr>';
			// Arbeitsstunden/Woche
			if($GLOBALS['personen'][$persid]['wochenarbeitstd'] > 0)
			{
				if($tooltip_js)
					$tooltip_js .= '<tr><td colspan=2><br><hr class=fxhr><br></td></tr>';
				$wsdez = get_entry($GLOBALS['personen'][$persid]['wochenarbeitstd'], 'dezimal');
				$tooltip_js .= '<tr><td nowrap><b>'.$GLOBALS['pb_string']['aw'].':</b></td><td nowrap>&nbsp;'.$wsdez.'</td></tr>';
			}
			else
				$disabled=true;
			// Arbeitstage
			if($GLOBALS['personen'][$persid]['arbeitstage'] > 0)
			{
				$atdez = get_entry($GLOBALS['personen'][$persid]['arbeitstage'], 'dezimal');
				$tooltip_js .= '<tr><td nowrap><b>'.$GLOBALS['pb_string']['at'].':</b></td><td nowrap>&nbsp;'.$atdez.'</td></tr>';
			}
			else
				$disabled=true;
			// Fähigkeiten
			$maart = db_values("SELECT DISTINCT mafaehigkeit_id, maart_id FROM ma_art_faehigkeiten WHERE personen_id=".$persid." AND mandanten_id=".$mid." ORDER BY maart_id, mafaehigkeit_id", '*');
			if(fxIsArray($maart))
			{
				if($tooltip_js)
				{
					$colspan	 = ' colspan=2';
					$tooltip_js .= '<tr><td'.$colspan.'><br><hr class=fxhr><br></td></tr>';
				}
				else
					$colspan = '';
				$maback = 0;
				$macnt = 0;
				foreach($maart as $fk)
				{
					if($macnt > 12)
						$tooltip_js .= '<tr><td nowrap'.$colspan.'>...</td></tr>';
					if($maback != $fk['maart_id'])
					{
						$ma = db_value("SELECT maart_name FROM ma_art WHERE maart_id=".$fk['maart_id']." AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id IN (0,".$mid.")");
						$tooltip_js .= '<tr><td nowrap'.$colspan.'><b>'.str_replace("'", "´", $ma).'</b></td></tr>';
						$maback = $fk['maart_id'];
					}
					$mf = db_value("SELECT mafaehigkeit_name FROM ma_faehigkeit WHERE mafaehigkeit_id=".$fk['mafaehigkeit_id']." AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id IN (0,".$mid.")");
					$tooltip_js .= '<tr><td nowrap'.$colspan.'>&middot;&nbsp;'.str_replace("'", "&apos;", $mf).'</td></tr>';
					$macnt++;
				}
			}
			$tooltip_js	.= '</table>';
			$tooltip_js = '<table width=100%><tr><td width=100% bgcolor=#dddddd>&nbsp;<b class=s4>'.strtr($GLOBALS['personen'][$persid]['fullname'], $GLOBALS['caption_strtr']).'</b>&nbsp;</td></tr></table><br>'.$tooltip_js;
		}

		// Farbe ermitteln
		if(!fxIsArray($GLOBALS['personen'][$persid]['projekte']))	// ...keinem Projekt zugeordnet
			$kt1 = 0;
		else
			$kt1 = (int)$GLOBALS['personen'][$persid]['kosten']['typ'];
		if(!$kt1)
			$kt2 = 'r0';
		else
			$kt2 = $kt1;

		$cc = 0;
		if(isset($GLOBALS['personen'][$persid]['kosten']) && ($GLOBALS['con'] || $GLOBALS['fxpglobals']['persdat']['roles'][-3] || $GLOBALS['fxpglobals']['persdat']['roles'][1] || $GLOBALS['fxpglobals']['persdat']['roles'][2]))
		{
			// Spalte: OK. -> Zuordnen
			$o_chk = '';
			if($GLOBALS['err'])	// Fehler
			{
				if($_POST['OK'][$persid])
					$o_chk = '1';
			}
			else if($GLOBALS['personen'][$persid]['paktiv'])	// MA zu Aufgabe zugeordnet
				$o_chk = '1';

			$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['text'] = fieldCheckbox('OK['.$persid.']', '', $o_chk, false, $disabled);
			if($GLOBALS['personen'][$persid]['paktiv'])
				$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['text'] .= fieldHidden('AZO['.$persid.']',1);
			if($pz > 1)
				$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['rowspan'] = $pz;
			$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['align']		= 'center';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['class']		= 'ek'.$kt1;
			$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['padding']		= '4px';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['typ']			= 'All';
			$cc++;
		}

		$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['text'] = '';
		$with_cal=false;
		if(isset($GLOBALS['personen'][$persid]['kosten']) && ($GLOBALS['fxpglobals']['tr'] == 43))	// Ressourcenplanung
		{
			$null_ev = get_entry(0, 'dezimal');

			$prj_ek = $null_ev;
			$prj_vk = $null_ev;
			if($GLOBALS['personen'][$persid]['kosten']['typ'] == 2)
			{
				$prj_ek = get_entry(round($GLOBALS['personen'][$persid]['kosten']['ek'], 2), 'dezimal');
				$prj_vk = get_entry(round($GLOBALS['personen'][$persid]['kosten']['vk'], 2), 'dezimal');
			}

			$pers_ek = get_entry(round($GLOBALS['personen'][$persid]['kosten']['s_ek'], 2), 'dezimal');
			$pers_vk = get_entry(round($GLOBALS['personen'][$persid]['kosten']['s_vk'], 2), 'dezimal');

			$ges_ek = $null_ev;
			if(isset($GLOBALS['ekgv']) && ($GLOBALS['ekgv'] > 0.00))
				$ges_ek = get_entry(round($GLOBALS['ekgv'], 2), 'dezimal');
			$ges_vk = $null_ev;
			if(isset($GLOBALS['vkgv']) && ($GLOBALS['vkgv'] > 0.00))
				$ges_vk = get_entry(round($GLOBALS['vkgv'], 2), 'dezimal');

			if($GLOBALS['budgetart'] != 'N')
			{
				$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['text'] .= '<img id="cal_'.$persid.'" src="'.$GLOBALS['gfxpath'].'cfg_cal.png" style="float:right;margin-right:2px;cursor:pointer;" tooltip="'.$GLOBALS['evzp_txt'][0].'" '.fxf_jsFunction('openResPPSP', $persid, $GLOBALS['personen'][$persid]['fullname'], $prj_ek, $prj_vk, $pers_ek, $pers_vk, $ges_ek, $ges_vk, 'event').'>';
				$with_cal=true;
			}
		}
		$bildchen=person_pic($persid, 20, $GLOBALS['personen'][$persid]['geschlecht'], $GLOBALS['fxpglobals']['client'], false, 'float:left;');
		$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['text'] .= substr($bildchen, 0, -1).' title="['.$GLOBALS['personen'][$persid]['mandanten_id'].', '.$persid.'] '.$GLOBALS['personen'][$persid]['fullname'].'">';
		// Prüfen, ob noch Mitarbeiter in Firma?
		$noex = '';
		if(($GLOBALS['personen'][$persid]['eintritt'] > $today.'000000') || ($GLOBALS['personen'][$persid]['austritt'] && ($GLOBALS['personen'][$persid]['austritt'] < $today.'240000')))
			$noex = 'X&nbsp;';
		// Farbe + Personentyp
		if(($GLOBALS['personen'][$persid]['eintritt'] > $GLOBALS['von']) || ($GLOBALS['personen'][$persid]['austritt'] && ($GLOBALS['personen'][$persid]['austritt'] < $GLOBALS['bis'])))
			$pclass	= 'red';
		else if($disabled_name || $noex)
			$pclass	= 'lightgrey';
		else if($GLOBALS['personen'][$persid]['ptyp'] == 840)	// Ext.
			$pclass	= 'darkgrey';
		else if($GLOBALS['personen'][$persid]['ptyp'] == 275)	// Int.
			$pclass	= 'black';
		else	// Ansprechpartner
			$pclass	= 'blue';
		if($GLOBALS['personentyparray'][$GLOBALS['personen'][$persid]['ptyp']])
			$ptyp = '&nbsp;<i class="normal s2">('.$GLOBALS['personentyparray'][$GLOBALS['personen'][$persid]['ptyp']].')</i>';
		else
			$ptyp = '';
		$name='&nbsp;<font class="'.$pclass.'">'.$noex.$GLOBALS['personen'][$persid]['fullname'].$ptyp.'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</font>';
		if(!isset($GLOBALS['rechte_205']))
			$GLOBALS['rechte_205']=tr_rechte(205);		// Personalstammblatt
		if($GLOBALS['rechte_205'])
			$name=fieldLink('hrinfo'.$persid, $name, 'loader.php?url=prs_info.inc&pid='.$persid);
		if($with_cal)
			$name .= '<font style="margin-right:48px;">&nbsp;</font>';
		$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['text'] .= $name;
		if($pz > 1)
			$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['rowspan'] = $pz;
		$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['class']		= 'ek'.$kt2;
		$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['padding']		= '4px';
		$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['typ']			= 'All';
		$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['tooltip_js']	= $tooltip_js;

		if(isset($GLOBALS['personen'][$persid]['kosten']))
		{
			$effort		= 0.00;
			$waehrung	= $GLOBALS['fxpglobals']['settings']['waehrung'];
			$nachkomma	= (int)$GLOBALS['fxpglobals']['settings']['nkstellen'];
			if($GLOBALS['fxpglobals']['settings']['deztrennzeichen'] == ',')
				$tsd_tz = '.';
			else
				$tsd_tz = ',';

			if(!$GLOBALS['praufg'])
			{
				// Spalte 1: Projektzugehörigkeit
				$cc++;
				//$persid, $GLOBALS['Projekt_ID']
				if(fxIsArray($GLOBALS['personen'][$persid]['projekte']) && $GLOBALS['personen'][$persid]['paktiv'])
				{
//fxDebug($GLOBALS['personen'][$persid]['projekt_zugeordnet']);
					$chk='1';
					$tooltip_js  = '<table border=0 cellpadding=0 cellspacing=0>';
					// Überschrift
					$tooltip_js .= '<tr>';
					$tooltip_js .= '<td class=rhd align=center nowrap>'.$GLOBALS['pb_string']['aufg'].'</td><td>&nbsp;</td>';
					$tooltip_js .= '<td class=rhd align=center nowrap>'.substr($GLOBALS['pb_string']['ausl'], 0, 4).'.</td><td>&nbsp;</td>';
					$tooltip_js .= '<td class=rhd align=center nowrap>%</td><td>&nbsp;</td>';
					$tooltip_js .= '<td class=rhd align=center nowrap>'.$GLOBALS['pb_string']['ek'].'</td><td>&nbsp;</td>';
					$tooltip_js .= '<td class=rhd align=center nowrap>'.$GLOBALS['pb_string']['vk'].'</td><td>&nbsp;</td>';
					$tooltip_js .= '<td class=rhd align=center nowrap>'.$GLOBALS['pb_string']['aufw'].'</td>';
					$tooltip_js .= '</tr>';

					$psa = 0.00;
					$ttl = 0;
					foreach($GLOBALS['personen'][$persid]['projekte'] as $prz)
					{
						if(!$prz['zugeordnet'])
							continue;

						// Max. 25 Aufgaben anzeigen
						if($ttl == 25)
						{
							$tooltip_js .= '<tr>';
							$tooltip_js .= '<td colspan=11 align=center nowrap><b>...</b></td>';
							$tooltip_js .= '</tr>';
						}
						else if($ttl < 25)
						{
							$ttc = 'rv'.($ttl%2);

							$vnr = no_null($prz['vorgangsnummer']).'&nbsp;';
							$tooltip_js .= '<tr>';
							$tooltip_js .= '<td class='.$ttc.' align=left nowrap>&nbsp;<b>'.$vnr.'</b>'.strtr($prz['name_projekt'], $GLOBALS['caption_strtr']).'&nbsp;</td><td></td>';
							$tooltip_js .= '<td class='.$ttc.' align=right nowrap>&nbsp;'.get_entry(($prz['auslastung']*100.00), 'dezimal').'&nbsp;%&nbsp;</td><td></td>';
							$tooltip_js .= '<td class='.$ttc.' align=right nowrap>&nbsp;'.get_entry(($prz['prozent']*100.00), 'dezimal').'&nbsp;%&nbsp;</td><td></td>';
							$tooltip_js .= '<td class='.$ttc.' align=right nowrap>&nbsp;'.get_entry($prz['ek'], 'dezimal').'&nbsp;'.$waehrung.'&nbsp;</td><td></td>';
							$tooltip_js .= '<td class='.$ttc.' align=right nowrap>&nbsp;'.get_entry($prz['vk'], 'dezimal').'&nbsp;'.$waehrung.'&nbsp;</td><td></td>';
							$tooltip_js .= '<td class='.$ttc.' align=right nowrap>&nbsp;'.get_entry(($prz['aufwand_sek']/3600), 'dezimal').'&nbsp;h&nbsp;</td>';
							$tooltip_js .= '</tr>';
						}

						$psa += $prz['prozent'];
						$ttl++;
					}
					// Summenzeile
					if((double)$pr_gesamtaufwand > 0.00)
					{
						$aufw = (string)round($GLOBALS['personen'][$persid]['aufwand_sek']*100.00 / (double)$pr_gesamtaufwand, 2);
						$effort = $aufw;
					}
					else if($ttl)
					{
						$aufw = (string)round($psa*100.00 / (double)$ttl, 2);
						$effort = $aufw;
					}
					else
						$aufw = (string)round(100.00/(double)$GLOBALS['pszugeordnet'], 2);
					$tooltip_js .= '<tr>';
					$tooltip_js .= '<td></td><td></td><td></td><td></td>';
					$tooltip_js .= '<td class=rs align=right nowrap>&nbsp;<b>'.get_entry($aufw, 'dezimal').'&nbsp;%</b>&nbsp;</td><td></td>';
					$tooltip_js .= '<td class=rs align=right nowrap>&nbsp;<b>'.get_entry($GLOBALS['personen'][$persid]['kosten']['ek'], 'dezimal').'&nbsp;'.$waehrung.'</b>&nbsp;</td><td></td>';
					$tooltip_js .= '<td class=rs align=right nowrap>&nbsp;<b>'.get_entry($GLOBALS['personen'][$persid]['kosten']['vk'], 'dezimal').'&nbsp;'.$waehrung.'</b>&nbsp;</td><td></td>';
					$tooltip_js .= '<td class=rs align=right nowrap>&nbsp;<b>'.get_entry(($GLOBALS['personen'][$persid]['aufwand_sek']/3600), 'dezimal').'&nbsp;h</b>&nbsp;</td>';
					$tooltip_js .= '</tr>';
					$tooltip_js	.= '</table>';
					$tooltip_js = '<table width=100%><tr><td width=100% bgcolor=#dddddd>&nbsp;<b class=s4>'.strtr($GLOBALS['pb_string']['info'], $GLOBALS['caption_strtr']).'</b>&nbsp;</td></tr></table><br>'.$tooltip_js;
				}
				else
				{
					$chk='';
					// Keinen untergeordneten Aufgaben zugeordnet!
					$tooltip_js = '<table width=100%><tr><td width=100% bgcolor=#dddddd>&nbsp;<b class=s4>'.strtr($GLOBALS['pb_string']['info'], $GLOBALS['caption_strtr']).'</b>&nbsp;</td></tr></table><br>'.$GLOBALS['pb_string']['kua'];
				}
				$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['class']		= 'ek'.$kt2;
				$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['text'] 		= fieldCheckbox('PZ['.$persid.']', '', $chk, false, true);
				if($pz > 1)
					$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['rowspan'] = $pz;
				$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['align']		= 'center';
				$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['padding']		= '4px';
				$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['tooltip_js']	= $tooltip_js;
				$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['typ']			= 'All';
			}

			if($GLOBALS['con'] || $GLOBALS['fxpglobals']['persdat']['roles'][-3] || $GLOBALS['fxpglobals']['persdat']['roles'][1] || $GLOBALS['fxpglobals']['persdat']['roles'][2])
			{
				// Spalte 2: EK
				if(($GLOBALS['budgetart'] == 'I') || ($GLOBALS['budgetart'] == 'B'))
				{
					$cc++;
					$oek=get_entry($GLOBALS['personen'][$persid]['kosten']['ek'], 'dezimal');
					if($GLOBALS['err'])	// Fehler
						$ek=$_POST['EK'][$persid];
					else
						$ek=$oek;

					$cek=validate_entry($ek, $verr, 'dezimal');
					if($cek == $GLOBALS['personen'][$persid]['kosten']['s_ek'])
						$icon = $GLOBALS['iekok'];
					else if($cek == $GLOBALS['personen'][$persid]['kosten']['d_ek'])
						$icon = $GLOBALS['iekdt'];
					else if(isset($GLOBALS['ekgv']) && ($GLOBALS['ekgv'] > 0.00) && ($cek == $GLOBALS['ekgv']))
						$icon = $GLOBALS['iekge'];
					else if($cek < $GLOBALS['personen'][$persid]['kosten']['s_ek'])
						$icon = $GLOBALS['iekkl'];
					else
						$icon = $GLOBALS['iekgr'];

					$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['text']  = fieldHidden('MID['.$persid.']',$GLOBALS['personen'][$persid]['mandanten_id']);
					$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['text'] .= fieldHidden('OEK['.$persid.']',$oek);
					$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['text'] .= '<span id="iconEK['.$persid.']">'.$icon.'</span>'.fieldFloat('EK['.$persid.']', $ek, 64, false, $disabled).'&nbsp;'.$waehrung.'&nbsp;';
					if($pz > 1)
						$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['rowspan'] = $pz;
					$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['align']	= 'right';
					$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['class']	= 'ek'.$kt2;
					$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['padding']	= '4px';
					$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['typ']		= 'All';
				}

				// Spalte 3: VK
				if(($GLOBALS['budgetart'] == 'E') || ($GLOBALS['budgetart'] == 'B'))
				{
					$cc++;
					$ovk=get_entry($GLOBALS['personen'][$persid]['kosten']['vk'], 'dezimal');
					if($GLOBALS['err'])	// Fehler
						$vk=$_POST['VK'][$persid];
					else
						$vk=$ovk;

					$cvk=validate_entry($vk, $verr, 'dezimal');
					if($cvk == $GLOBALS['personen'][$persid]['kosten']['s_vk'])
						$icon = $GLOBALS['ivkok'];
					else if($cvk == $GLOBALS['personen'][$persid]['kosten']['d_vk'])
						$icon = $GLOBALS['ivkdt'];
					else if(isset($GLOBALS['vkgv']) && ($GLOBALS['vkgv'] > 0.00) && ($cvk == $GLOBALS['vkgv']))
						$icon = $GLOBALS['ivkge'];
					else if($cvk < $GLOBALS['personen'][$persid]['kosten']['s_vk'])
						$icon = $GLOBALS['ivkkl'];
					else
						$icon = $GLOBALS['ivkgr'];

					$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['text']  = fieldHidden('OVK['.$persid.']',$ovk);
					$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['text'] .= '<span id="iconVK['.$persid.']">'.$icon.'</span>'.fieldFloat('VK['.$persid.']', $vk, 64, false, $disabled).'&nbsp;'.$waehrung.'&nbsp;';
					if($pz > 1)
						$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['rowspan'] = $pz;
					$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['align']	= 'right';
					$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['class']	= 'ek'.$kt2;
					$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['padding']	= '4px';
					$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['typ']		= 'All';
				}

				// Spalte 4: Aufwand
				$cc++;
				$aufw	= '';
				$oaufw	= '';
				if($GLOBALS['err'])	// Fehler
				{
					$aufw	= $_POST['AUFW'][$persid];
					$oaufw	= $_POST['OAUFW'][$persid];
				}
				else if($GLOBALS['personen'][$persid]['paktiv'])
				{
					if($effort)
						$aufw = (string)get_entry($effort, 'dezimal');
					else if((double)$pr_gesamtaufwand > 0.00)
						$aufw = (string)get_entry(round($GLOBALS['personen'][$persid]['aufwand_sek']*100.00/(double)$pr_gesamtaufwand, 2), 'dezimal');
					else if($GLOBALS['praufg'])
						$aufw = (string)get_entry(round($GLOBALS['personen'][$persid]['prozent']*100.00, 2), 'dezimal');
					else
						$aufw = (string)get_entry(round(100.00/(double)$GLOBALS['pszugeordnet'], 2), 'dezimal');

					$oaufw = $aufw;
				}

				$icon = '';
				if($GLOBALS['personen'][$persid]['d_aufw'])	// Durchschnitts-Aufwand
					$icon = '<img src="'.$GLOBALS['gfxpath'].'rpdt.png" hspace="2" vspace="0" tooltip="Delta-'.$GLOBALS['pb_string']['aufw'].'" style="cursor:help;">';

				$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['text']  = fieldHidden('OAUFW['.$persid.']',$oaufw);
				$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['text'] .= $icon.'&nbsp;'.fieldFloat('AUFW['.$persid.']', $aufw, 48, false, $disabled).'&nbsp;%&nbsp;';
				if($pz > 1)
					$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['rowspan'] = $pz;
				$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['align']	= 'right';
				$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['class']	= 'ek'.$kt2;
				$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['padding']	= '4px';
				$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['typ']		= 'All';
			}
		}

		if($_POST['ZAuslastung'])
		{
			// Zeile x (Spalte 1: "Auslastung")
			$cc++;
			$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['text']	= '&nbsp';
			if($pz > 1)
				$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['rowspan'] = $pz;
			$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['align']	= 'right';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['class']	= 'ekr0';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['padding']	= '4px';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$cc]['typ']		= 'All';
			$cc_ausl = $cc;
			$cc_aktzeile = $aktzeile;
		}
		else
			$cc_ausl = 0;

		// Projekte oder Auslastung
		$prdrawn = false;
		if($GLOBALS['personen'][$persid]['projekte']) // Mitarbeiter ist mind. 1 Projekt zugeordnet
		{
//echo('('.__LINE__.') <b>MA zu Projekten zugeordnet</b><HR>');
			foreach($GLOBALS['personen'][$persid]['projekte'] as $pkey => $value)
			{
				$psdate	= min(max(substr($value['soll_beginn'], 0, 8), $GLOBALS['darr'][0]['info']['mindate']), $GLOBALS['darr'][0]['info']['maxdate']);
				$pedate	= max(min(substr($value['ber_ende'], 0, 8), $GLOBALS['darr'][0]['info']['maxdate']), $GLOBALS['darr'][0]['info']['mindate']);
				$pscol	= $GLOBALS['xoffset']+$GLOBALS['oarr'][$psdate];
				$pecol	= $GLOBALS['xoffset']+$GLOBALS['oarr'][$pedate];
//echo($aktzeile.': $psdate='.$psdate.', $pedate='.$pedate.' - $pscol='.$pscol.', $pecol='.$pecol.' - $GLOBALS[\'xoffset\']='.$GLOBALS['xoffset'].', $GLOBALS[\'xcolumns\']='.$GLOBALS['xcolumns'].'<br>');

				if($_POST['ZProjekte'] && $value['disp'])
				{
					$prdrawn = true;
					if($pscol > $GLOBALS['xoffset'])
					{
						$GLOBALS['fxptdata']['cal'][$aktzeile][$GLOBALS['xoffset']]['text']			= '~';
						$GLOBALS['fxptdata']['cal'][$aktzeile][$GLOBALS['xoffset']]['background']	= 'pr999';
						$GLOBALS['fxptdata']['cal'][$aktzeile][$GLOBALS['xoffset']]['colspan']		= $pscol-$GLOBALS['xoffset'];
					}
					projekt($GLOBALS['personen'][$persid], $pkey, $aktzeile, $ausl_array, $term_array, true);
					if($pecol < ($GLOBALS['xcolumns']-1))
					{
						$GLOBALS['fxptdata']['cal'][$aktzeile][$pecol+1]['text']		= '~';
						$GLOBALS['fxptdata']['cal'][$aktzeile][$pecol+1]['background']	= 'pr999';
						$GLOBALS['fxptdata']['cal'][$aktzeile][$pecol+1]['colspan']		= $GLOBALS['xcolumns']-1-$pecol;
					}
					$aktzeile++;
				}
				else if($_POST['ZAuslastung'])
					projekt($GLOBALS['personen'][$persid], $pkey, $aktzeile, $ausl_array, $term_array, false);
			}
		}
		if(!$prdrawn && $_POST['ZProjekte']) // Mitarbeiter zu keinem Projekt zugeteilt
		{
//echo('('.__LINE__.') <b>MA zu keinen Projekten zugeordnet</b><HR>');
			$GLOBALS['fxptdata']['cal'][$aktzeile][$GLOBALS['xoffset']]['text']			= '~---';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$GLOBALS['xoffset']]['tooltip_js']	= $ka_string['kp'];
			$GLOBALS['fxptdata']['cal'][$aktzeile][$GLOBALS['xoffset']]['background']	= 'pr999';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$GLOBALS['xoffset']]['colspan']		= $GLOBALS['xcolumns']-$GLOBALS['xoffset'];
			$aktzeile++;
		}
//fxDebug($ausl_array, '$ausl_array (KOMPLETT): '.__LINE__);

		// Auslastungen anzeigen aktiviert
		if($_POST['ZAuslastung'])
		{
//echo('('.__LINE__.') <b>Auslastung ermitteln</b><HR>');
			verfuegbar($GLOBALS['xoffset'], $GLOBALS['xcolumns']-$GLOBALS['xoffset']+1, $aktzeile, $ausl_array, $term_array, $einheit);

			$aktzeile++;
			if(($einheit == 'A') || ($einheit == 'D'))
				$aktzeile++;
		}

		if(!$_POST['ZAuslastung'] && !$_POST['ZProjekte'])
			$aktzeile++;

//fxDebug($GLOBALS['personen'][$persid], $GLOBALS['personen'][$persid]['fullname']);

		// Durchschnittsauslastung ermitteln
		if($_POST['ZAuslastung'])
		{
//fxDebug($ausl_array, '$ausl_array');
			if(!isset($GLOBALS['delta_ausl_array']))
				$GLOBALS['delta_ausl_array'] = $ausl_array;
			else
			{
				foreach($ausl_array as $off => $data)
				{
					foreach($data as $dkey => $dval)
					{
						if($dkey == 'typ')
						{
							$gval = $GLOBALS['delta_ausl_array'][$off][$dkey];
							if(($dval == 'x') || ($dval == 'c'))
								$GLOBALS['delta_ausl_array'][$off][$dkey] = $dval;
						}
						else if($dkey != 'datum')
							$GLOBALS['delta_ausl_array'][$off][$dkey] += $dval;
					}
				}
			}
		}

		// Gesamtauslastung ermitteln
		if($cc_ausl && sizeof($ausl_array))
		{
//fxDebug($ausl_array, '$ausl_array'.__LINE__);

			// Wochenverfügbarkeit ermitteln
			foreach($ausl_array as $aa)
			{
				$GLOBALS['personen'][$persid]['maus']['astd_ges']	+= $aa['astd_ges'];
				$GLOBALS['personen'][$persid]['maus']['astd_akt']	+= $aa['astd_akt'];
				$GLOBALS['personen'][$persid]['maus']['astd_gen']	+= $aa['astd_gen'];
				$GLOBALS['personen'][$persid]['maus']['astd_gepl']	+= $aa['astd_gepl'];

				$GLOBALS['delta_maus']['astd_ges']					+= $aa['astd_ges'];
				$GLOBALS['delta_maus']['astd_akt']					+= $aa['astd_akt'];
				$GLOBALS['delta_maus']['astd_gen']					+= $aa['astd_gen'];
				$GLOBALS['delta_maus']['astd_gepl']					+= $aa['astd_gepl'];
			}
//fxDebug($GLOBALS['personen'][$persid]['maus'], 'Auslastung nach Berechnung: '.__LINE__);

			for($dm=0; $dm<2; $dm++)
			{
				if(!$dm)
					$maus = $GLOBALS['personen'][$persid]['maus'];
				else
					$maus = $GLOBALS['delta_maus'];

				// Gesamtauslastung berechnen
				$sstd_ges	= round($maus['gstd']*100.0)/100.0;
				$gstd_ges	= round($maus['astd_ges']*100.0)/100.0;

				$gstd_akt	= round(($maus['astd_term']+$maus['astd_akt'])*100.0)/100.0;
				$gstd_gen	= round(($maus['astd_term']+$maus['astd_akt']+$maus['astd_gen'])*100.0)/100.0;
				$gstd_gepl	= round(($maus['astd_term']+$maus['astd_akt']+$maus['astd_gen']+$maus['astd_gepl'])*100.0)/100.0;

				if($sstd_ges)
				{
					$gausl_ges	= ceil($gstd_ges*100.00 / $sstd_ges);

					$gausl_akt	= ceil($gstd_akt*100.00 / $sstd_ges);
					$gausl_gen	= ceil($gstd_gen*100.00 / $sstd_ges);
					$gausl_gepl	= ceil($gstd_gepl*100.00 / $sstd_ges);
				}
				else
				{
					$gausl_ges	= 0.00;

					$gausl_akt	= 0.00;
					$gausl_gen	= 0.00;
					$gausl_gepl	= 0.00;
				}

				if(!$dm)
				{
					$txt  = '<table border=0 cellpadding=0 cellspacing=0 style=margin:4px>';
					$txt .= '<tr>';
					$txt .= '<td align=center valign=bottom style="border-bottom:1px solid black;font-size:7pt;">('.get_entry($maus['astd_term'], "dezimal").'&nbsp;+&nbsp;'.get_entry($maus['astd_ges']-$maus['astd_term'], "dezimal").')</td>';
					$txt .= '<td rowspan=2 align=center valign=middle>&nbsp;=&nbsp;'.get_entry($gausl_ges, "dezimal").'%&nbsp;<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></td>';
					$txt .= '</tr>';
					$txt .= '<tr>';
					$txt .= '<td align=center valign=top style="font-size:7pt;">'.get_entry($sstd_ges, "dezimal").'</td>';
					$txt .= '</tr>';
					$txt .= '</table>';
				}
				else
					$txt  = get_entry($gausl_ges, "dezimal").'%&nbsp;<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top">&nbsp;';

				$width=72;
				// Termine,Aktiv,Genehm.,Geplant,Ist-Std.,Soll-Std.,Auslast.
				if(!isset($GLOBALS['usum_headers']))
					$GLOBALS['usum_headers'] = explode(',', meldung(2449, true));
				$tt  = '<table border=0 bgcolor=#bbbbbb cellspacing=1 cellpadding=2>';
				$tt .= '<tr>';
				foreach($GLOBALS['usum_headers'] as $head)
					$tt .= '<td width='.$width.' class=rhd align=center nowrap>'.str_replace(' ', '&nbsp;', $head).'</td>';
				$tt .= '</tr>';
				$tt .= '<tr>';
				$tt .= '<td bgcolor=#f8f8f8 align=right nowrap>&nbsp;'.get_entry($maus['astd_term'], "dezimal").'&nbsp;h&nbsp;</td><td bgcolor=#f8f8f8 align=right nowrap>&nbsp;<font color=#000000>'.get_entry($maus['astd_akt'], "dezimal").'&nbsp;h</font>&nbsp;</td><td bgcolor=#e8e8e8 align=right nowrap>&nbsp;</td><td bgcolor=#e8e8e8 align=right nowrap>&nbsp;</td><td bgcolor=#f8f8f8 align=right nowrap>&nbsp;'.get_entry($gstd_akt, "dezimal").'&nbsp;h&nbsp;</td><td bgcolor=#f8f8f8 align=right nowrap>&nbsp;'.get_entry($sstd_ges, "dezimal").'&nbsp;h&nbsp;</td><td bgcolor=#f8f8f8 align=right nowrap>&nbsp;'.get_entry($gausl_akt, "dezimal").'&nbsp;%&nbsp;</td>';
				$tt .= '</tr>';
				$tt .= '<tr>';
				$tt .= '<td bgcolor=#f8f8f8 align=right nowrap>&nbsp;'.get_entry($maus['astd_term'], "dezimal").'&nbsp;h&nbsp;</td><td bgcolor=#f8f8f8 align=right nowrap>&nbsp;<font color=#000000>'.get_entry($maus['astd_akt'], "dezimal").'&nbsp;h</font>&nbsp;</td><td bgcolor=#f8f8f8 align=right nowrap>&nbsp;<font color=#006b9f>'.get_entry($maus['astd_gen'], "dezimal").'&nbsp;h</font>&nbsp;</td><td bgcolor=#e8e8e8 align=right nowrap>&nbsp;</td><td bgcolor=#f8f8f8 align=right nowrap>&nbsp;'.get_entry($gstd_gen, "dezimal").'&nbsp;h&nbsp;</td><td bgcolor=#f8f8f8 align=right nowrap>&nbsp;'.get_entry($sstd_ges, "dezimal").'&nbsp;h&nbsp;</td><td bgcolor=#f8f8f8 align=right nowrap>&nbsp;'.get_entry($gausl_gen, "dezimal").'&nbsp;%&nbsp;</td>';
				$tt .= '</tr>';
				$tt .= '<tr>';
				$tt .= '<td bgcolor=#f8f8f8 align=right nowrap>&nbsp;'.get_entry($maus['astd_term'], "dezimal").'&nbsp;h&nbsp;</td><td bgcolor=#f8f8f8 align=right nowrap>&nbsp;<font color=#000000>'.get_entry($maus['astd_akt'], "dezimal").'&nbsp;h</font>&nbsp;</td><td bgcolor=#f8f8f8 align=right nowrap>&nbsp;<font color=#006b9f>'.get_entry($maus['astd_gen'], "dezimal").'&nbsp;h</font>&nbsp;</td><td bgcolor=#f8f8f8 align=right nowrap>&nbsp;<font color=#787878>'.get_entry($maus['astd_gepl'], "dezimal").'&nbsp;h</font>&nbsp;</td><td bgcolor=#f8f8f8 align=right nowrap>&nbsp;'.get_entry($gstd_gepl, "dezimal").'&nbsp;h&nbsp;</td><td bgcolor=#f8f8f8 align=right nowrap>&nbsp;'.get_entry($sstd_ges, "dezimal").'&nbsp;h&nbsp;</td><td bgcolor=#f8f8f8 align=right nowrap>&nbsp;'.get_entry($gausl_gepl, "dezimal").'&nbsp;%&nbsp;</td>';
				$tt .= '</tr>';
				$tt .= '<tr>';
				$tt .= '<td bgcolor=#d3e0e6 align=right nowrap>&nbsp;<b>'.get_entry($maus['astd_term'], "dezimal").'&nbsp;h</b>&nbsp;</td><td colspan=3 bgcolor=#d3e0e6 align=center nowrap>&nbsp;<b>'.get_entry($maus['astd_ist'], "dezimal").'&nbsp;h&nbsp;(ZE)&nbsp;&nbsp;+&nbsp;&nbsp;'.get_entry($maus['astd_ges']-$maus['astd_term']-$maus['astd_ist'], "dezimal").'&nbsp;h&nbsp;&nbsp;=&nbsp;&nbsp;'.get_entry($maus['astd_ges']-$maus['astd_term'], "dezimal").'&nbsp;h</b>&nbsp;</td><td bgcolor=#d3e0e6 align=right nowrap>&nbsp;<b>'.get_entry($gstd_ges, "dezimal").'&nbsp;h</b>&nbsp;</td><td bgcolor=#d3e0e6 align=right nowrap>&nbsp;<b>'.get_entry($sstd_ges, "dezimal").'&nbsp;h</b>&nbsp;</td><td bgcolor=#d3e0e6 align=right nowrap>&nbsp;<b>'.get_entry($gausl_ges, "dezimal").'&nbsp;%</b>&nbsp;</td>';
				$tt .= '</tr>';
				$tt .= '</table>';

				if(!$dm)
				{
					$GLOBALS['fxptdata']['cal'][$cc_aktzeile][$cc_ausl]['text'] = $txt;
					$GLOBALS['fxptdata']['cal'][$cc_aktzeile][$cc_ausl]['ausl'] = get_entry($gausl_ges, "dezimal").'%';
					$GLOBALS['fxptdata']['cal'][$cc_aktzeile][$cc_ausl]['tooltip_js'] = $tt;
				}
				else
				{
					$GLOBALS['delta_maus']['cal']['text'] = $txt;
					$GLOBALS['delta_maus']['cal']['ausl'] = get_entry($gausl_ges, "dezimal").'%';
					$GLOBALS['delta_maus']['cal']['tooltip_js'] = $tt;
				}
			}
		}
	} // END -> if (Personen-Darstellung)
}

/**
 * Draw the available status
 *
 * @param various $spalte     - Mandatory parameter:                     ???PARAMETER???
 * @param various $colspan    - Mandatory parameter:                     ???PARAMETER???
 * @param various $aktzeile   - Mandatory parameter:                     ???PARAMETER???
 * @param various $ausl_array - Mandatory parameter:                     ???PARAMETER???
 * @param array   $term_array - Optional parameter (default = array()):  ???PARAMETER???
 * @param string  $einheit    - Optional parameter (default = 'D'):      ???PARAMETER???
 */
function verfuegbar($spalte, $colspan, $aktzeile, $ausl_array, $term_array=array(), $einheit='D')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(fxIsArray($ausl_array))
	{
		$today=substr($GLOBALS['datetime'],0,8);
		$terminzeile=$aktzeile;
		$auslastungszeile=$aktzeile;

		if(($einheit == 'A') || ($einheit == 'D'))
			$auslastungszeile++;

//fxDebug($ausl_array, '$ausl_array');
		foreach($ausl_array as $an => $ad)
		{
			$ap = 0.00;
			if($ad['sstd'])
				$ap = $ad['astd_ges']/$ad['sstd'];

			$pausl	= ceil($ap*100.00);
			$bg		= '';
			$txt	= '';
			$tt		= '';

			if(!$ad['sstd'])
			{
				if(($einheit == 'A') || ($einheit == 'D') || ($einheit == 'X'))
				{
					if($ausl_array[$an]['typ'] == 'w')
						$bg = 'au_ud';
					else
					{
						$bg = 'au_n';
						if($ad['datum'] >= $today)
							$tt = $GLOBALS['fxptdata']['backup']['legend'.$GLOBALS['fxpglobals']['lang']]['pb_string']['nv'];
					}
				}
				else if($ad['datum'] >= $today)
					$bg = 'au000';
				else
					$bg = 'ap000';
			}
			else
			{
				if($ad['datum'] >= $today)
				{
					if($ap == 0.0)
						$bg = 'au000';
					else if($ap < 0.15)
						$bg = 'au010';
					else if($ap > 2.0)
						$bg = 'au_x';
					else
					{
						$gausl = 10*round($ap*10);
						$bg = 'au'.substr('000'.$gausl, -3);
					}
				}
				else
				{
					if($ap == 0.0)
						$bg = 'ap000';
					else if($ap < 0.15)
						$bg = 'ap010';
					else if($ap > 2.0)
						$bg = 'au_x';
					else
					{
						$gausl = 10*round($ap*10);
						if($gausl >= 100)
							$bg = 'au'.substr('000'.$gausl, -3);
						else
							$bg = 'ap'.substr('000'.$gausl, -3);
					}
				}
				if($pausl > 0)
				{
					$tt = $pausl.'%';
					if($pausl < 1000)
						$tt = substr('00'.$pausl, -3).'%';
				}
				else if($ad['datum'] >= $today)
					$tt = $GLOBALS['fxptdata']['backup']['legend'.$GLOBALS['fxpglobals']['lang']]['pb_string']['vf'];
			}

			// Terminzeile
			if(($einheit == 'A') || ($einheit == 'D'))
			{
//fxDebug($ad, '$ad');
				$typ = substr($ad['typ'], 0, 1);

				// ... nicht taggenau
				if($typ == 'c')
				{
					if(isset($ad['colspan']))
					{
						$tcs = $ad['colspan'];

						$GLOBALS['fxptdata']['cal'][$terminzeile][$an]['text']			= '~';
						$GLOBALS['fxptdata']['cal'][$terminzeile][$an]['background']	= 'pr999';
						if($tcs > 1)
							$GLOBALS['fxptdata']['cal'][$terminzeile][$an]['colspan']	= $tcs;
					}
				}

				// ... taggenau
				else
				{
					$ttt = '';
					$tbg = 'pr000';

					if($typ != 'x')
					{
						if($typ == 'w')
							$tbg = 'au_ud';
						else if($typ == 'h')
							$tbg = 'au_ft';
						else if($typ == 's')
						{
							$tbg = 'au'.substr($ad['typ'], 1);
							if($ad['sstd'])
								$tbg .= '_2';
						}
						else
						{
							$tgausl	= 0.00;
							if($ad['sstd'])
								$tgausl	= $ad['astd_term']/$ad['sstd'];
							$tcausl	= ceil($tgausl*100);
							$ttt	= get_entry($tgausl*100, 'dezimal').' %';
							if(!$tcausl)
								$tbg = 'pr000';
							else if($tgausl < 0.15)
								$tbg = 'pr010';
							else if($tgausl > 1.00)
								$tbg = 'pr_ub';
							else
							{
								$tgausl = 10*round($tgausl*10);
								$tbg = 'pr'.substr('000'.$tgausl, -3);
							}
						}

						// Tooltip
						if($typ != 'w')
						{
							if(!isset($GLOBALS['app_icons']))
								$GLOBALS['app_icons'] = getAppIcons(true);

							$tad = $term_array[$ad['datum']];
//fxDebug($tad, 'Tooltip: '.$ad['datum']);
							$ttt = '<table border=0 cellspacing=0 cellpadding=0>';
							if(is_array($tad['hd']))
							{
								$ttt .= '<tr><td nowrap>&nbsp;'.$tad['hd']['name'].' ('.$tad['hd']['typ'].')&nbsp;</td></tr>';
							}
							if(is_array($tad['sd']))
							{
								$ttt .= '<tr><td><table border=0 cellspacing=0 cellpadding=2>';
								foreach($tad['sd'] as $stype => $sinfo)
								{
									if($stype != -1)
									{
										$ttt .= '<tr><td bgcolor=#dddddd nowrap><img src='.$GLOBALS['gfxpath'].$GLOBALS['app_icons'][$stype].'.png></td><td bgcolor=#eeeeee nowrap><b>'.$GLOBALS['app_icons']['name_'.$stype].':</b>&nbsp;</td><td nowrap align=right>&nbsp;('.get_entry($sinfo[-1]['stunden_termin'], "dezimal").'&nbsp;h)&nbsp;</td></tr>';
									}
								}
								$ttt .= '</table></td></tr>';
							}
							if(is_array($tad['ap']))
							{
								$ttt .= '<tr><td><table border=0 cellspacing=0 cellpadding=2>';
								foreach($tad['ap'] as $tadate => $tainfo)
								{
									if(strlen($tadate) > 4)
									{
										$tds = get_entry($tainfo['start'], "zeit");
										$tde = get_entry($tainfo['ende'], "zeit");
										if($tde != $tds)
											$tds .= '-'.$tde;
										$ttt .= '<tr><td bgcolor=#dddddd nowrap><img src='.$GLOBALS['gfxpath'].$GLOBALS['app_icons'][$tainfo['art']].'.png></td><td bgcolor=#eeeeee nowrap><b>'.$GLOBALS['app_icons']['name_'.$tainfo['art']].':</b>&nbsp;</td><td nowrap>&nbsp;'.$tds.'&nbsp;</td><td nowrap align=right>&nbsp;('.get_entry($tainfo['stunden_termin'], "dezimal").'&nbsp;h)&nbsp;</td></tr>';
									}
								}
								$ttt .= '</table></td></tr>';
							}
							$ttt .= '</table>';
						}
					}

					$GLOBALS['fxptdata']['cal'][$terminzeile][$an]['text'] = '~';
					if(strlen($tbg))
						$GLOBALS['fxptdata']['cal'][$terminzeile][$an]['background'] = $tbg;
					if(strlen($ttt))
						$GLOBALS['fxptdata']['cal'][$terminzeile][$an]['tooltip_js'] = $ttt;
				}
			}

			// Auslastung
			if(!$txt)
				$txt = '~';
			$GLOBALS['fxptdata']['cal'][$auslastungszeile][$an]['text'] = $txt;
			if($txt != '~')
				$GLOBALS['fxptdata']['cal'][$auslastungszeile][$an]['align'] = 'center';
			if(strlen($tt))
				$GLOBALS['fxptdata']['cal'][$auslastungszeile][$an]['tooltip_js'] = $tt;
			if(strlen($bg))
				$GLOBALS['fxptdata']['cal'][$auslastungszeile][$an]['background'] = $bg;
		}
	}
}

/**
 * Draw the project
 *
 * @param various $person      - Mandatory parameter:                     ???PARAMETER???
 * @param various $pkey        - Mandatory parameter:                     ???PARAMETER???
 * @param various $aktzeile    - Mandatory parameter:                     ???PARAMETER???
 * @param various &$ausl_array - Mandatory parameter:                     ???PARAMETER???
 * @param array   $term_array  - Optional parameter (default = array()):  ???PARAMETER???
 * @param boolean $calmode     - Optional parameter (default = true):     ???PARAMETER???
 */
function projekt($person, $pkey, $aktzeile, &$ausl_array, $term_array=array(), $calmode=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $prio_array, $farbnamen, $pb_string;

	// Projekt
	if(is_array($person) && is_array($person['projekte']) && is_array($person['projekte'][$pkey]))
	{
		$projekt = $person['projekte'][$pkey];
//fxDebug($projekt, '$projekt');

		$sdate	= min(max(substr($projekt['soll_beginn'], 0, 8), $GLOBALS['darr'][0]['info']['mindate']), $GLOBALS['darr'][0]['info']['maxdate']);
		$edate	= max(min(substr($projekt['ber_ende'], 0, 8), $GLOBALS['darr'][0]['info']['maxdate']), $GLOBALS['darr'][0]['info']['mindate']);
		$spalte	= $GLOBALS['xoffset']+$GLOBALS['oarr'][$sdate];
		$cspan	= 1+$GLOBALS['xoffset']+$GLOBALS['oarr'][$edate]-$spalte;
//echo('$sdate='.$sdate.', $edate='.$edate.', $spalte='.$spalte.', $cspan='.$cspan.'<br>');

		$mode = 1;
		if($_POST['nur_plandaten'])
			$mode = 0;
		$uar = getUtilization($person, $pkey, $term_array, $mode);
		if($calmode && $projekt['disp'])
		{
			$gausl = $uar['delta_auslastung'];
			$pausl = get_entry($gausl*100, 'dezimal');
			$causl = ceil($gausl*100);
			$GLOBALS['fxptdata']['cal'][$aktzeile][$spalte]['text'] = $projekt['name_projekt'];
			if(tr_rechte(23))
				$GLOBALS['fxptdata']['cal'][$aktzeile][$spalte]['link'] = 'newtr&inittr=true&tr=23&aktion=1&_projektid='.(int)$projekt['projekt_id'].'&_mutterprojektid='.(int)$projekt['mutterprojekt_id'];
			if($projekt['zugeordnet'])
				$GLOBALS['fxptdata']['cal'][$aktzeile][$spalte]['font'] = 'class="bold"';
			if(!$causl)
			{
				if($GLOBALS['fxptdata']['cal'][$aktzeile][$spalte]['font'])
					$GLOBALS['fxptdata']['cal'][$aktzeile][$spalte]['font'] .= ' ';
				$GLOBALS['fxptdata']['cal'][$aktzeile][$spalte]['font'] .= 'color="#881122"';
			}
			if(isset($projekt['atyp']))
			{
				if($projekt['atyp'] == 1)		// geplant ohne Budgetgenehmigung
					$bcol = '787878';
				else if($projekt['atyp'] == 2)	// geplant mit Budgetgenehmigung
					$bcol = '005077';
				else							// aktiv
					$bcol = '000000';
//echo('<b>atyp:</b> '.$projekt['atyp'].' -&gt; background-color=['.$bgcol.']<br>');

				if($GLOBALS['fxptdata']['cal'][$aktzeile][$spalte]['font'])
					$GLOBALS['fxptdata']['cal'][$aktzeile][$spalte]['font'] .= ' ';
				$GLOBALS['fxptdata']['cal'][$aktzeile][$spalte]['font'] .= 'style="position:absolute;padding:0 4px;margin:1px;margin-top:-8px;border-right:1px solid #444444;border-bottom:1px solid #444444;color:#'.$bcol.';background:#ffffff;border-radius:4px;"';
			}

			// Name
			$tooltip_js	= '<table border=0 cellspacing=0 cellpadding=1>';
			// Pfad
			$tta = tt_path($projekt['projekt_id']);
			if(is_array($tta))
				$tooltip_js .= '<tr><td nowrap valign=top><b>'.$pb_string['pt'].':</b>&nbsp;</td><td nowrap>'.$tta['stooltip'].'</td></tr>';
			$tooltip_js .= '<tr><td colspan=2><br><hr class=fxhr><br></td></tr>';
			// Soll-Beginn / Soll-Ende
			$tooltip_js .= '<tr><td nowrap valign=top><b>'.$pb_string['sb'].':</b>&nbsp;</td><td nowrap>'.get_entry($projekt['soll_beginn'], 'datum').'</td></tr>';
			$tooltip_js .= '<tr><td nowrap valign=top><b>'.$pb_string['se'].':</b>&nbsp;</td><td nowrap>';
			if($projekt['ber_ende'] > $projekt['soll_ende'])
				$tooltip_js .= '<i>'.get_entry($projekt['soll_ende'], 'datum').'</i> -&gt; <b class=red>'.get_entry($projekt['ber_ende'], 'datum').'</b>';
			else
				$tooltip_js .= get_entry($projekt['soll_ende'], 'datum');
			$tooltip_js .= '</td></tr>';
			// Ist-Aufwand / Soll-Aufwand / Restaufwand
			if($projekt['aufwand_sek'] < 900000)	// Kleiner 250 Std. -> Stunde(n)
			{
				$ai = round($projekt['ist_sek']/3600.00, 2);
				$as = round($projekt['aufwand_sek']/3600.00, 2);
				$ar = round(($projekt['aufwand_sek']-$projekt['ist_sek'])/3600.00, 2);
				$einh = $pb_string['stunden'];
			}
			else									// -> Tag(e)
			{
				$ai = round($projekt['ist_sek']/($GLOBALS['fxpglobals']['settings']['mtag']*3600.00), 2);
				$as = round($projekt['aufwand_sek']/($GLOBALS['fxpglobals']['settings']['mtag']*3600.00), 2);
				$ar = round(($projekt['aufwand_sek']-$projekt['ist_sek'])/($GLOBALS['fxpglobals']['settings']['mtag']*3600.00), 2);
				$einh = $pb_string['tage'];
			}
			$tooltip_js .= '<tr><td nowrap valign=top><b>'.$pb_string['isa'].':</b>&nbsp;</td><td nowrap>'.get_entry($ai, 'dezimal').'&nbsp;/&nbsp;'.get_entry($as, 'dezimal').'&nbsp;'.$einh.'</td></tr>';
			// Restaufwand
			$tooltip_js .= '<tr><td nowrap valign=top><b>'.$pb_string['ra'].':</b>&nbsp;</td><td nowrap>'.get_entry($ar, 'dezimal').'&nbsp;'.$einh.'</td></tr>';
			// Priorität
			$tooltip_js .= '<tr><td nowrap valign=top><b>'.$pb_string['prio'].':</b>&nbsp;</td><td nowrap>'.$prio_array[$projekt['prioritaet']].'</td></tr>';
			// Beschreibung
			if(isset($projekt['beschreibung']) && strlen($projekt['beschreibung']))
			{
				$rep			= $GLOBALS['caption_strtr'];
				$rep["\r\n"]	= '<br>';
				$rep["\n"]		= '<br>';
				$desc			= strtr(text_wrap(trim($projekt['beschreibung']), true, 100), $rep);

				$tooltip_js .= '<tr><td nowrap valign=top><b>'.$pb_string['beschr'].':</b>&nbsp;</td><td>'.$desc.'</td></tr>';
			}
			// Auslastung
			$tooltip_js .= '<tr><td colspan=2><br><hr class=fxhr></td></tr>';
			$tooltip_js .= '<tr><td nowrap bgcolor=#f0f0ff valign=top><b>'.$pb_string['ausl'].':</b>&nbsp;</td><td nowrap bgcolor=#f0f0ff>';
			if(!$causl)
				$tooltip_js .= '<font class=red>'.$pausl.' %</font>';
			else
				$tooltip_js .= $pausl.' %';
			$tooltip_js .= '</td></tr>';
			$tooltip_js	.= '</table>';
			$tooltip_js = '<table width=100%><tr><td width=100% bgcolor=#dddddd>&nbsp;<b class=s4>'.no_null($projekt['vorgangsnummer']).'&nbsp;-&nbsp;'.strtr($projekt['name_projekt'], $GLOBALS['caption_strtr']).'</b>&nbsp;</td></tr></table><br>'.$tooltip_js;

			if(!$causl)
				$bg = 'pr000';
			else if($gausl < 0.15)
				$bg = 'pr010';
			else if($gausl > 1.00)
				$bg = 'pr_ub';
			else
			{
				$gausl = 10*round($gausl*10);
				$bg = 'pr'.substr('000'.$gausl, -3);
			}

			$GLOBALS['fxptdata']['cal'][$aktzeile][$spalte]['tooltip_js']	= $tooltip_js;
			$GLOBALS['fxptdata']['cal'][$aktzeile][$spalte]['colspan']		= $cspan;
			$GLOBALS['fxptdata']['cal'][$aktzeile][$spalte]['background']	= $bg;

			if($projekt['ber_ende'] > $projekt['soll_ende'])
				$GLOBALS['fxptdata']['cal'][$aktzeile][$spalte]['ts']		= 1;
		}

		if(is_array($uar) && sizeof($uar) && $uar['sollstunden_pro_tag'] && is_array($uar['tages_sollstunden']))
		{
			foreach($uar['tages_sollstunden'] as $udate => $uvalue)
			{
				if(isset($GLOBALS['oarr'][$udate]))
				{
					$off = $GLOBALS['xoffset']+$GLOBALS['oarr'][$udate];

					if($projekt['disp'])
						$ausl_array[$off]['astd_ges'] += $uvalue;

					if(isset($projekt['atyp']))
					{
						if($projekt['atyp'] == 1)		// geplant ohne Budgetgenehmigung
							$ausl_array[$off]['astd_gepl'] += $uvalue;
						else if($projekt['atyp'] == 2)	// geplant mit Budgetgenehmigung
							$ausl_array[$off]['astd_gen'] += $uvalue;
						else							// aktiv
							$ausl_array[$off]['astd_akt'] += $uvalue;
					}
				}
			}
		}
//fxDebug($ausl_array, '$ausl_array');
	}
}

/**
 * Draw calendar days
 *
 * @param various $von     - Mandatory parameter:                  ???PARAMETER???
 * @param various $bis     - Mandatory parameter:                  ???PARAMETER???
 * @param string  $einheit - Optional parameter (default = 'D'):   ???PARAMETER???
 * @param boolean $dsplit  - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function kalendertage($von, $bis, $einheit='D', $dsplit=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!isset($GLOBALS['darr']) || !is_array($GLOBALS['darr']))
		dateSplit($von, $bis, $einheit, $dsplit);
	if(!isset($GLOBALS['darr']) || !is_array($GLOBALS['darr']))
		return 0;

	$today=substr($GLOBALS['datetime'],0,8);

	$this_week	= kw($today);
	$sod		= sizeof($GLOBALS['darr']);
	$left		= $GLOBALS['darr'][0]['info']['cyears_l']+$GLOBALS['darr'][0]['info']['cmonths_l']+$GLOBALS['darr'][0]['info']['cweeks_l']+$GLOBALS['darr'][0]['info']['cdays_l'];
	$right		= $sod-$left;
	$ftart		= array(FXP_HL_FULL=>' [1]', FXP_HL_HALF=>' [½]', FXP_HL_DISPLAY=>' [0]');
	$quarter	= array(1=>'I', 2=>'II', 3=>'III', 4=>'IV');
	$txt_months	= fxf_getReferenceValues('monate');				// 1=Januar, 2=Februar, ..., 12=Dezember
	$txt_wdays	= fxf_getWeekdayNames();				// 0=Sonntag, 1=Montag, ..., 6=Samstag
	$txt_cw		= meldung(1036, true);					// Kalenderwoche-Kurzzeichen "KW"
	$txt_quart	= substr(meldung(1586, true), 0, 1);	// Quartal-Kurzzeichen "Q"
	if(!isset($GLOBALS['holidays']))
		$GLOBALS['holidays'] = get_feiertage($von, $bis, $GLOBALS['fxpglobals']['client']);
//echo('$today='.$today.', $sod='.$sod.', $left='.$left.', $right='.$right.'<br>');

	// Spaltenbreite
	if($GLOBALS['fxpglobals']['tr'] == 264)	// Leistungsarten-Report
		$wa=array(40,40,40,50,50,60);
	else
		$wa=array(28,22,31,40,49,58);
	$width=$wa[0];
	if($einheit == 'D')
		$width=$wa[1];
	else if($einheit == 'W')
		$width=$wa[2];
	else if($einheit == 'M')
		$width=$wa[3];
	else if($einheit == 'Q')
		$width=$wa[4];
	else if($einheit == 'Y')
		$width=$wa[5];

	// Zeile 2: Tag/Woche/Monat/Quartal/Jahr
//fxDebug($GLOBALS['darr'], '$darr: einheit='.$einheit);
	$ca=array('ffffff','f8f8f8');
	foreach($GLOBALS['darr'] as $doffset => $d)
	{
		// Font color
		$fc='color="#3b3b3b"';		// Future
		if($dsplit && ($doffset < $left))
			$fc='color="#787878"';	// Past
		// Tooltip
		$tt='';
		// Spaltentext
		// ...Quartal: Q1, Q2, ...
		if($d['type'] == 'quart')
			$txt=$txt_quart.$d['quart'];
		// ...Jahr: 2013, 2014, ...
		else if($d['type'] == 'year')
			$txt=$d['year'];
		// ...Woche, Monat, Tag: 01, 02, ...
		else
			$txt=substr('00'.$d[$d['type']], -2);
		// ...Monat: Jan., Feb., ...
		if($d['type'] == 'month')
		{
			$txt=$txt_months[(int)$txt];
			if(strlen($txt) > 4)
				$txt=substr($txt,0,3).'.';
		}
		// ...Woche: K01, K02, ...
		if($d['type'] == 'week')
			$txt=substr($txt_cw,0,1).$txt;

		if($d['type'] == 'day')
		{
			if($d['date'] == $today)	// Heute
			{
				$col='d3e0e6';
				$tt=get_text(540,'20');
				$fc='color="#000000"';
			}
			else if(isset($GLOBALS['holidays']) && isset($GLOBALS['holidays'][$d['date']]))	// Feiertag
			{
				if(!$GLOBALS['holidays'][$d['date']]['typ'])
					$col='ffffdd';
				else
					$col='ffeebb';
				$tt=$GLOBALS['holidays'][$d['date']]['name'].$ftart[$GLOBALS['holidays'][$d['date']]['art']];
			}
			else if(!$d['wday'] || ($d['wday'] > 5))	// Wochenende
				$col='f0f0f0';
			else
				$col='ffffff';
		}
		else
			$col=$ca[$d[$d['type']]%2];

		$GLOBALS['fxptdata']['cal'][2][$GLOBALS['xoffset']+$doffset]['text']='<div style="width:'.$width.'px;overflow:hidden;">'.$txt.'</div>';
		$GLOBALS['fxptdata']['cal'][2][$GLOBALS['xoffset']+$doffset]['width']=$width;
		$GLOBALS['fxptdata']['cal'][2][$GLOBALS['xoffset']+$doffset]['align']='center';
		$GLOBALS['fxptdata']['cal'][2][$GLOBALS['xoffset']+$doffset]['color']=$col;
		if(strlen($fc))
			$GLOBALS['fxptdata']['cal'][2][$GLOBALS['xoffset']+$doffset]['font']=$fc;
		if(strlen($tt))
			$GLOBALS['fxptdata']['cal'][2][$GLOBALS['xoffset']+$doffset]['tooltip']=$tt;
		$GLOBALS['fxptdata']['cal'][2][$GLOBALS['xoffset']+$doffset]['typ']='All';
	}

	// Zeile 1: Tag->Woche, Woche->Monat, Monat->Quartal, Quartal->Jahr, Jahr->(leer)
	$ca=array('efefef','e8e8e8');
	$doffset=0;
	while($doffset < $sod)
	{
		// Font color
		$fc='color="#3b3b3b"';		// Future
		if($dsplit && ($doffset < $left))
			$fc='color="#787878"';	// Past

		$soffset=$doffset;
		$cs=1;
		$type=$GLOBALS['darr'][$doffset]['type'];

		// Jahr -> (leer)
		if($type == 'year')
		{
			$txt=' ';
			$col=$ca[0];
			$cs=$sod;
		}
		// Quartal -> Jahr
		else if($type == 'quart')
		{
			$year=$GLOBALS['darr'][$soffset]['year'];
			$txt=$year;
			$col=$ca[$year%2];

			$doffset++;
			while(($doffset < $sod) && ($GLOBALS['darr'][$doffset]['type'] == $type) && ($GLOBALS['darr'][$doffset]['year'] == $year))
			{
				$cs++;
				$doffset++;
			}
		}
		// Monat -> Quartal
		else if($type == 'month')
		{
			$quart=$GLOBALS['darr'][$soffset]['quart'];
			$txt=$txt_quart.$quart;
			$col=$ca[$quart%2];

			$doffset++;
			while(($doffset < $sod) && ($GLOBALS['darr'][$doffset]['type'] == $type) && ($GLOBALS['darr'][$doffset]['quart'] == $quart))
			{
				$cs++;
				$doffset++;
			}
		}
		// Woche -> Monat
		else if($type == 'week')
		{
			$month=$GLOBALS['darr'][$soffset]['month'];
			$txt=$txt_months[$month];
			$col=$ca[$month%2];

			$doffset++;
			while(($doffset < $sod) && ($GLOBALS['darr'][$doffset]['type'] == $type) && ($GLOBALS['darr'][$doffset]['month'] == $month))
			{
				$cs++;
				$doffset++;
			}

			if((($cs == 1) && (strlen($txt) > 4)) || (($cs == 2) && (strlen($txt) > 7)))
				$txt = substr($txt,0,3).'.';
		}
		// Tag -> Woche
		else
		{
			$month=$GLOBALS['darr'][$soffset]['month'];
			$week=$GLOBALS['darr'][$soffset]['week'];
			$txt=$txt_cw.' '.substr('00'.$week, -2);
			$col=$ca[$week%2];

			if($week == $this_week)
				$fc='color="#000000"';

			$doffset++;
			while(($doffset < $sod) && ($GLOBALS['darr'][$doffset]['type'] == $type) && ($GLOBALS['darr'][$doffset]['week'] == $week) && ($GLOBALS['darr'][$doffset]['month'] == $month))
			{
				$cs++;
				$doffset++;
			}
			if($cs < 2)
				$txt=substr($txt, 3);
		}

		if($GLOBALS['fxpglobals']['tr'] == 264)
			$txt=' '.$txt;
		$GLOBALS['fxptdata']['cal'][1][$GLOBALS['xoffset']+$soffset]['text']=$txt;
		$GLOBALS['fxptdata']['cal'][1][$GLOBALS['xoffset']+$soffset]['colspan']=$cs;
		$GLOBALS['fxptdata']['cal'][1][$GLOBALS['xoffset']+$soffset]['color']=$col;
		if(strlen($fc))
			$GLOBALS['fxptdata']['cal'][1][$GLOBALS['xoffset']+$soffset]['font']=$fc;

		if($type == 'year')
			break;
	}

	// Zeile 0: Tag->Monat, Woche->Quartal, Monat->Jahr, Quartal+Jahr->(leer)
	$ca=array('dfdfdf','d8d8d8');
	$doffset=0;
	while($doffset < $sod)
	{
		// Font color
		$fc='color="#3b3b3b"';		// Future
		if($dsplit && ($doffset < $left))
			$fc='color="#787878"';	// Past

		$soffset=$doffset;
		$cs=1;
		$type=$GLOBALS['darr'][$doffset]['type'];

		// Jahr/Quartal -> (leer)
		if(($type == 'year') || ($type == 'quart'))
		{
			$txt=' ';
			$col=$ca[0];
			$cs=$sod;
		}
		// Monat -> Jahr
		else if($type == 'month')
		{
			$year=$GLOBALS['darr'][$soffset]['year'];
			$txt=$year;
			$col=$ca[$year%2];

			$doffset++;
			while(($doffset < $sod) && ($GLOBALS['darr'][$doffset]['type'] == $type) && ($GLOBALS['darr'][$doffset]['year'] == $year))
			{
				$cs++;
				$doffset++;
			}
		}
		// Woche -> Quartal
		else if($type == 'week')
		{
			$quart=$GLOBALS['darr'][$soffset]['quart'];
			$txt=$txt_quart.$quart.' '.$GLOBALS['darr'][$soffset]['year'];
			$col=$ca[$quart%2];

			$doffset++;
			while(($doffset < $sod) && ($GLOBALS['darr'][$doffset]['type'] == $type) && ($GLOBALS['darr'][$doffset]['quart'] == $quart))
			{
				$cs++;
				$doffset++;
			}
			if($cs == 1)
				$txt=substr($txt,0,2);
		}
		// Tag -> Monat
		else
		{
			$month=$GLOBALS['darr'][$soffset]['month'];
			$txm=$txt_months[$month];
			$txt=$txm.' '.$GLOBALS['darr'][$soffset]['year'];
			$col=$ca[$month%2];

			if($GLOBALS['darr'][$soffset]['year'].substr('00'.$month, -2) == substr($today,0,6))
				$fc='color="#000000"';

			$doffset++;
			while(($doffset < $sod) && ($GLOBALS['darr'][$doffset]['type'] == $type) && ($GLOBALS['darr'][$doffset]['month'] == $month))
			{
				$cs++;
				$doffset++;
			}
			if($cs == 1)
			{
				if(strlen($txm) > 3)
					$txt=substr($txm,0,3).'.';
				else
					$txt=$txm;
			}
			else if($cs == 2)
			{
				if(strlen($txm) > 6)
					$txt=substr($txm,0,3).'.';
				else
					$txt=$txm;
			}
			else if($cs == 3)
			{
				if(strlen($txm) > 6)
					$txt=substr($txm,0,3).'.';
				else
					$txt=$txm;
				$txt .= ' '.substr($GLOBALS['darr'][$soffset]['year'],-2);
			}
			else if($cs == 4)
			{
				if(strlen($txm) > 6)
					$txt=substr($txm,0,3).'.';
				else
					$txt=$txm;
				$txt .= ' '.$GLOBALS['darr'][$soffset]['year'];
			}
		}

		if($GLOBALS['fxpglobals']['tr'] == 264)
			$txt=' '.$txt;
		$GLOBALS['fxptdata']['cal'][0][$GLOBALS['xoffset']+$soffset]['text']=$txt;
		$GLOBALS['fxptdata']['cal'][0][$GLOBALS['xoffset']+$soffset]['colspan']=$cs;
		$GLOBALS['fxptdata']['cal'][0][$GLOBALS['xoffset']+$soffset]['color']=$col;
		if(strlen($fc))
			$GLOBALS['fxptdata']['cal'][0][$GLOBALS['xoffset']+$soffset]['font']=$fc;

		if(($type == 'year') || ($type == 'quart'))
			break;
	}

	return ($sod+$GLOBALS['xoffset']);
}

/**
 * Draw the color legend
 *
 * @param string  $do  - Optional parameter (default = 'get'): ???PARAMETER???
 * @param boolean $all - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function farblegende($do='get', $all=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $nl;

	// Backup-Variablen erstellen
	if($do == 'vars')
	{
		// $pb_string für Kontrollzentrum, MA-Verfügbarkeit und Urlaubsplan
		if(!is_array($GLOBALS['fxptdata']['backup']['legend'.$GLOBALS['fxpglobals']['lang']]['pb_string']))
		{
			// Texte aus Datenbank holen
			$pb_string = array(
				'aktuell'		=> meldung(1144),			// Aktuelle Projekte/Aufgaben
				'zeitspanne'	=> meldung(1105),			// Zeitspanne
				'mitarbeiter'	=> meldung(1364),			// Mitarbeiter
				'mand'			=> meldung(1507),			// Mand.
				'info'			=> meldung(1106),			// Info
				'aufg'			=> meldung(1475),			// Aufgaben
				'pj'			=> get_text(631, '5'),		// Prj.
				'ek'			=> get_text(805, '5'),		// EK
				'vk'			=> get_text(806, '5'),		// VK
				'aufw'			=> get_text(1280, '5'),		// Aufw.
				'ok'			=> '+',						// + -> Zuordnen
				'ko'			=> '-',						// - -> Entziehen

				'pt'			=> meldung(1160),			// Pfad
				'sb'			=> meldung(1230),			// SOLL-Beginn
				'se'			=> meldung(1231),			// SOLL-Ende
				'isa'			=> meldung(1315),			// Ist-/Soll-Aufw.
				'ra'			=> get_text(1298, '20'),	// Restaufwand
				'stunden'		=> meldung(916),			// Stunde(n)
				'tage'			=> meldung(917),			// Tag(e)
				'prio'			=> get_text(47, '10'),		// Priorität
				'ausl'			=> get_text(135, '10'),		// Auslastung
				'aw'			=> get_text(59, '15'),		// Arbeitsstunden/Woche
				'fae'			=> get_text(375, '15'),		// Fähigkeit
				'at'			=> get_text(230, '15'),		// Arbeitstage
				'kunde'			=> get_text(766, '10'),		// Kunde
				'pl'			=> get_text(121, '10'),		// Projektleiter
				'ansp'			=> get_text(1195, '10'),	// Ansprechpartner
				'prs'			=> get_text(137, '10'),		// Projekt-Status
				'dead'			=> get_text(48, '10'),		// Deadline
				'mz'			=> get_text(132, '10'),		// max.Zeitaufwand
				'beschr'		=> get_text(38, '20'),		// Beschreibung
				'glt'			=> get_text(928, '20'),		// Gleitzeit

				'ka'			=> meldung(1107),			// (Keine Angaben)
				'kp'			=> meldung(1303),			// (Keinen aktiven Vorgängen mit Auslastung>0 zugeteilt)
				'kal'			=> meldung(1304),			// (Keine Auslastung definiert)
				'nv'			=> meldung(1132),			// Nicht Verfügbar
				'vf'			=> meldung(770),			// Verfügbar
				'kua'			=> meldung(1589)			// Keinen untergeordneten Aufgaben zugeordnet!
			);
			// und im Backup-Array zeischenspeichern
			$GLOBALS['fxptdata']['backup']['legend'.$GLOBALS['fxpglobals']['lang']]['pb_string']	= $pb_string;
		}

		// $prio_array
		if(!is_array($GLOBALS['fxptdata']['backup']['legend'.$GLOBALS['fxpglobals']['lang']]['prio_array']))
		{
			// Prioritäten
			$pia = db_values("SELECT wertetabid, tabwert FROM wertetab WHERE id_feld=47 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0", '*');
			if(is_array($pia))
			{
				foreach($pia as $value)
					$prio_array[$value['wertetabid']] = $value['tabwert'];
			}
			// und im Backup-Array zeischenspeichern
			$GLOBALS['fxptdata']['backup']['legend'.$GLOBALS['fxpglobals']['lang']]['prio_array'] = $prio_array;
		}

		// $farbbalken + $farbnamen
		if(!is_array($GLOBALS['fxptdata']['backup']['legend'.$GLOBALS['fxpglobals']['lang']]['farbbalken']))
		{
			// Farblegende
			$farbbalken = db_values("SELECT wertetabid, tabwert FROM wertetab WHERE id_feld=582 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0 ORDER BY position", '*');
			if(is_array($farbbalken))
			{
				foreach($farbbalken as $value)
					$farbnamen[$value['wertetabid']] = $value['tabwert'];
			}
			$farbnamen['tooltip'] = array(
				FXP_AT_ABSENT=>2880,													// Abwesend
				FXP_AT_VACATIONREQUEST=>1245,			FXP_AT_VACATION=>767,			// Urlaub (Antr.)		+ Urlaub
				FXP_AT_SPECIALVACATIONREQUEST=>2881,	FXP_AT_SPECIALVACATION=>2882,	// Sonderurl. (Antr.)	+ Sonderurl.
				FXP_AT_TRAININGREQUEST=>2005,			FXP_AT_TRAINING=>2107,			// Schulung (Antr.)		+ Schulung
				FXP_AT_SICKCALL=>768,					FXP_AT_SICK=>2108,				// Krank (Mldg.)		+ Krank
				FXP_AT_SLIDINGDAYREQUEST=>2385,			FXP_AT_SLIDINGDAY=>2407			// Gleittag (Antr.)		+ Gleittag
			);
			// und im Backup-Array zeischenspeichern
			$GLOBALS['fxptdata']['backup']['legend'.$GLOBALS['fxpglobals']['lang']]['farbbalken'] = $farbbalken;
			$GLOBALS['fxptdata']['backup']['legend'.$GLOBALS['fxpglobals']['lang']]['farbnamen']  = $farbnamen;
		}
	}

	// Farblegende erstellen
	else
	{
		$legende = '';
		if($all)
		{
			$fs = 0;
			$twidth = ' width="110"';
			$dwidth = '2';
		}
		else
		{
			$fs = 5;
			$twidth = '';
			$dwidth = '8';
		}
		$fe = 16;
		$farbbalken_grafik = array(
			1132 => 'au100',	770  => 'au000',	2465 => 'au_ud',	1070 => 'au200',	772  => 'pr100',
			2880 => 'au'.FXP_AT_ABSENT,															// Abwesend
			1245 => 'au'.FXP_AT_VACATIONREQUEST, 		767  => 'au'.FXP_AT_VACATION,			// Urlaub
			2881 => 'au'.FXP_AT_SPECIALVACATIONREQUEST, 2882 => 'au'.FXP_AT_SPECIALVACATION,	// Sonderurlaub
			2005 => 'au'.FXP_AT_TRAININGREQUEST,		2107 => 'au'.FXP_AT_TRAINING,			// Schulung
			768  => 'au'.FXP_AT_SICKCALL,				2108 => 'au'.FXP_AT_SICK,					// Krank
			2385 => 'au'.FXP_AT_SLIDINGDAYREQUEST,		2407 => 'au'.FXP_AT_SLIDINGDAY			// Gleittag
		);
		$theight=13;
		if(is_array($GLOBALS['fxptdata']['backup']['legend'.$GLOBALS['fxpglobals']['lang']]['farbbalken']))
		{
			$farbbalken = $GLOBALS['fxptdata']['backup']['legend'.$GLOBALS['fxpglobals']['lang']]['farbbalken'];
			$legende .= '<table border=0 cellpadding=0 cellspacing=1>'.$nl;
			$legende .= '	<tr>'.$nl;
			$inz = false;
			for($fcnt=$fs; $fcnt<$fe; $fcnt++)
			{
				if($all && (($farbbalken[$fcnt]['wertetabid'] == 1245) || ($farbbalken[$fcnt]['wertetabid'] == 768)))
				{
					$legende .= '		<td>&nbsp;</td>'.$nl;
					$legende .= '	</tr>'.$nl;
					$legende .= '</table>'.$nl;
					$legende .= '<table border=0 cellpadding=0 cellspacing=1>'.$nl;
					$legende .= '	<tr>'.$nl;
					$inz = false;
				}
				if($inz)
					$legende .= '		<td><img src="'.$GLOBALS['gfxpath'].'dmy.png" width="'.$dwidth.'" height="1" border="0" align="center"></td>'.$nl;
				$legende .= '		<td'.$twidth.' align="right" nowrap><span class="s2 grey">'.$GLOBALS['fxptdata']['backup']['legend'.$GLOBALS['fxpglobals']['lang']]['farbnamen'][$farbbalken[$fcnt]['wertetabid']].'</span>&nbsp;</td><td background="'.$GLOBALS['gfxpath'].strtolower($farbbalken_grafik[$farbbalken[$fcnt]['wertetabid']]).'.png" style="border:1px solid #bbbbbb;border-radius:4px;"><img src="'.$GLOBALS['gfxpath'].'dmy.png" width="11" height="'.$theight.'"></td>'.$nl;
				$inz = true;
			}
			$legende .= '<td>&nbsp;</td>'.$nl;
			$legende .= '</tr>'.$nl;
			$legende .= '</table><br>'.$nl;
		}
		return $legende;
	}
}

/**
 * Get day abbreviations in a language, like "L" for "leave" or "T" for "Training" etc.
 *
 * @param various $dtype - Mandatory parameter:               ???PARAMETER???
 * @param integer $lng   - Optional parameter (default = 0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getDayAppr($dtype,$lng=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$lng=(int)$lng;
	if(!$lng)
		$lng=$GLOBALS['fxpglobals']['lang'];

	$appr='';
	// Leave
	if(($dtype == FXP_AT_VACATIONREQUEST) || ($dtype == FXP_AT_VACATION) || ($dtype == FXP_AT_SPECIALVACATIONREQUEST) || ($dtype == FXP_AT_SPECIALVACATION))
	{
		$appr='L';
		if($lng == 1)
			$appr='U';
	}
	// Training
	else if(($dtype == FXP_AT_TRAININGREQUEST) || ($dtype == FXP_AT_TRAINING))
	{
		$appr='T';
		if($lng == 1)
			$appr='S';
	}
	// Sick
	else if(($dtype == FXP_AT_SICKCALL) || ($dtype == FXP_AT_SICK))
	{
		$appr='S';
		if($lng == 1)
			$appr='K';
	}
	// Flextime
	else if(($dtype == FXP_AT_SLIDINGDAYREQUEST) || ($dtype == FXP_AT_SLIDINGDAY))
	{
		$appr='F';
		if($lng == 1)
			$appr='G';
	}
	// Absent
	else if($dtype == FXP_AT_ABSENT)
	{
		$appr='A';
	}

	return $appr;
}
?>