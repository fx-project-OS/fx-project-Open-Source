<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : mask.inc                                                     //
// Version     : 21.1                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2021-08-12                                                   //
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
 * Function collection for creating and handling each fx-project mask
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;


////////////////////////////////////////////////////////////////////////////////
// VARIABLES
////////////////////////////////////////////////////////////////////////////////
$GLOBALS['msk_wfac']=1.00;			// Default mask zoom factor
$GLOBALS['mh_lit']='literal_20'; 	// Literal type to use for help
$GLOBALS['negnumber']=false;		// Variable to show if a number is negative or not
$GLOBALS['columncounter']=0;		// Mask column variables: counter, spacer
$GLOBALS['columncountermax']=0;
$GLOBALS['columnspacer']=false;
$GLOBALS['trcounter']=0;

$GLOBALS['mskfilter']=0;

$GLOBALS['_filter_literals']='';

$GLOBALS['gmskfilter']=0;
$GLOBALS['listheaderc']=0;

// Border colors (normal + sum)
$GLOBALS['msk_nbcol']='c4c4c4';
$GLOBALS['msk_tbcol']='a4a4a4';
$GLOBALS['msk_sbcol']='646464';

// Field display types
$GLOBALS['anzart_defines']=array('???', 'literal_5', 'literal_10', 'literal_15', 'literal_20', 'nur lesen-textfeld', 'textfeld, einzeilig', 'textfeld, mehrzeilig', 'auswahlfeld', 'bild', 'versteckt', 'checkbox', 'submit-button', 'reset-button', 'summe', 'icon', 'datei', 'submit-bild', 'literal_rahmen', 'literal_header', 'literal_text');
$GLOBALS['anzart_disptxt']=array('Undefined [0]', 'Literal 5 [1]', 'Literal 10 [2]', 'Literal 15 [3]', 'Literal 20 [4]', 'Read Only [5]', 'Text [6]', 'Textarea [7]', 'Select [8]', 'Image [9]', 'Hidden [10]', 'Checkbox [11]', 'Submit Button [12]', 'Reset Button [13]', 'Sum [14]', 'Icon [15]', 'File [16]', 'Submit Image [17]', 'Literal Frame [18]', 'Literal Header [19]', 'Literal Text [20]');


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * Create the HTML code for a specific mask
 *
 * @param various $mnr       - Mandatory parameter:                   Mask number
 * @param string  $headline  - Optional parameter (default = ''):     Overwrite default headline with this text
 * @param boolean $withclose - Optional parameter (default = false):  Display a close icon?
 *
 * @return HTML code of the mask
 */
function maske($mnr, $headline='', $withclose=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Global variables
	global $maskenfeldwerte;
	global $form;

	// Linux Bugfix?
	$GLOBALS['_masknumber']=(string)(trim($mnr));

	// Maske nicht anzeigen, falls $maske???_nv gesetzt ist
	if($GLOBALS['_masknumber'] > 0)
		$maske_not_visible='maske'.$GLOBALS['_masknumber'].'_nv';
	else
		$maske_not_visible='maske_'.abs($GLOBALS['_masknumber']).'_nv';	// case for user defined forms, having a negative form id
	if(isset($GLOBALS[$maske_not_visible]) && $GLOBALS[$maske_not_visible])
	{
		if(isset($GLOBALS['fxpvars']['lookups'][$GLOBALS['_masknumber']]))
			unset($GLOBALS['fxpvars']['lookups'][$GLOBALS['_masknumber']]);
		$GLOBALS['_maskcounter']++;
		return '';
	}

	$GLOBALS['fxpvars']['bbuffer'][$GLOBALS['_masknumber']]='';
	$GLOBALS['msfcnt']=0;
	$GLOBALS['tropen']=-666;
	$GLOBALS['tdopen']=-666;
	$GLOBALS['mttabs']='							';
	$GLOBALS['bttabs']=$GLOBALS['mttabs'].'						';
	$GLOBALS['ittabs']=$GLOBALS['bttabs'];
	$body_closed=false;

	$GLOBALS['check_baseline']=true;
	if(fxIsArray($_POST) && isset($_POST['mt_mb']) && $_POST['mt_mb'])
		$GLOBALS['check_baseline']=false;

	$zusatz='';			// Zusatz-Parameter für Weitergabe mittels POST
	$d_manager=true;	// Datei-Manager verfügbar

	// Maskenausrichtung ermöglichen, wenn $maske???_al angegeben
	$msk_align='maske'.$GLOBALS['_masknumber'].'_al';
	if(isset($GLOBALS[$msk_align]) && (gettype($GLOBALS[$msk_align]) == 'string'))
		$msk_align=' align="'.$GLOBALS[$msk_align].'"';
	else
		$msk_align='';

	// Keinen Zeilenumbruch erzwingen
	$msk_nowrap='maske'.$GLOBALS['_masknumber'].'_nw';
	if(isset($GLOBALS[$msk_nowrap]) && (gettype($GLOBALS[$msk_nowrap]) == 'boolean'))
		$msk_nowrap=' NOWRAP';
	else
		$msk_nowrap='';

	// Keine Spaltenüberschriften anzeigen
	$maske_noheadlines='maske'.$GLOBALS['_masknumber'].'_nch';
	if(isset($GLOBALS[$maske_noheadlines]) && $GLOBALS[$maske_noheadlines])
	{
		$maske_noheadlines=true;
		$zusatz .= '|pdf_noheadlines';
		$maske_setborders='maske'.$GLOBALS['_masknumber'].'_sbd';
		if(!isset($GLOBALS[$maske_setborders]) || !$GLOBALS[$maske_setborders])
			$zusatz .= '|pdf_noborders';
	}
	else
		$maske_noheadlines=false;

	// PDF-Report immer weiss
	$maske_allwhite='maske'.$GLOBALS['_masknumber'].'_aw';
	if(isset($GLOBALS[$maske_allwhite]) && $GLOBALS[$maske_allwhite])
		$zusatz .= '|pdf_allwhite';

	// Keine Waehrung
	if(isset($GLOBALS['no_waehrung']) && $GLOBALS['no_waehrung'])
		$zusatz .= '|pdf_nocurrency';

	// Identifier ermitteln als Transaktions-ID
	if(isset($GLOBALS['fxpglobals']['tr']))
		$mskidname=$GLOBALS['fxpglobals']['tr'];
	else
		$mskidname=0;
	if(isset($GLOBALS['fxptdata']['marray']) && !isset($GLOBALS['fxptdata']['marray'][$mskidname]))
	{
		clearOldSessionMasks();
		$GLOBALS['fxptdata']['marray']=array();
		$GLOBALS['mfieldsinfo']=array();
	}

	// Checkbox und Literale merken
	if(!fxIsArray($GLOBALS['fxptdata']['litf_'.$GLOBALS['_masknumber']]))
		$GLOBALS['fxptdata']['litf_'.$GLOBALS['_masknumber']]=array();
	if(!fxIsArray($GLOBALS['fxptdata']['litc_'.$GLOBALS['_masknumber']]))
		$GLOBALS['fxptdata']['litc_'.$GLOBALS['_masknumber']]=array();
	if(!fxIsArray($GLOBALS['fxptdata']['chkf_'.$GLOBALS['_masknumber']]))
		$GLOBALS['fxptdata']['chkf_'.$GLOBALS['_masknumber']]=array();

	// Nounits löschen
	$GLOBALS['fxptdata']['nounits_'.$GLOBALS['_masknumber']]=array();

	// Variable zum Merken der max. anzuzeigenden Einträge
	$maxcounts=0;

	// Auswahlmaskenfelder
	$msk_sf_hidden='';
	$sql="SELECT auswahlmaske FROM masken WHERE nummer=".(int)$GLOBALS['_masknumber']." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
	$sfe=(int)db_value($sql);
//fxDebug($sfe,$sql, 0);
	if($sfe)
	{
		$msk_sf_nr=(int)$GLOBALS['_masknumber'];
		$msk_sf_hidden=fieldHidden('sf'.(int)$GLOBALS['fxpglobals']['tr'].'_set',$msk_sf_nr);

		// Alle Auswahlfelder ermitteln
		if(!fxIsArray($GLOBALS['fx_mfields'][$msk_sf_nr]))
			sessionMask($msk_sf_nr);
		if(fxIsArray($GLOBALS['fx_mfields'][$msk_sf_nr]))
		{
			// Folgende Anzeigearten ('t'=Text, 's'=Select, 'c'=Checkbox)
			$msk_sf_anz=array(6=>'t', 7=>'t', 8=>'s', 11=>'c');
			$msk_sf_rem=array();
			foreach($GLOBALS['fx_mfields'][$msk_sf_nr] as $f)
			{
				// Folgende Felder überspringen: Falsche Anzeigeart, bereits bearbeitet oder ausgeblendet
				if(!fxIsArray($f) || !isset($f['anzeigeart']) || !isset($msk_sf_anz[$f['anzeigeart']]) || $msk_sf_rem[$f['feldname']] || $GLOBALS[$f['feldname'].'_nv'])
					continue;

				$msk_sf_rem[$f['feldname']]=true;

				$hfn='sf'.$msk_sf_nr.'_'.$f['feldname'];
				$msk_sf_hidden .= fieldHidden($hfn,$msk_sf_anz[$f['anzeigeart']]);
			}
		}
	}
//echo('$msk_sf_hidden=<PRE>'.fxHtmlEncode($msk_sf_hidden).'</PRE>');

	// Spaltenauswahl für Reportmasken (nur bei JavaScript)
	$msk_rc_hidden='';
	$msk_rc_nr='';
	$msk_rc_fnr='';
	$msk_rc_disp='';
	$msk_rc_fields='';
	$msk_rc_tr=(int)$GLOBALS['fxpglobals']['tr'];
	if($msk_rc_tr < 0)
		$msk_rc_tr='u'.abs($msk_rc_tr);
	if(isset($GLOBALS['no_columnselect']) && $GLOBALS['no_columnselect'])
		$rcn=0;
	else if(($GLOBALS['_masknumber'] == 466) && (substr($msk_rc_tr, 0, 1) == 'u') && ($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) && isset($GLOBALS['_mgmsknr']))
		$rcn=1000000000 - (int)$GLOBALS['_mgmsknr'];
	else
	{
		$sql="SELECT param FROM trans_maske WHERE transakt_id=".(int)$GLOBALS['fxpglobals']['tr']." AND nummer=".(int)$GLOBALS['_masknumber'];
		$rcn=(int)db_value($sql);
	}
	if($rcn)
	{
		$msk_rc_nr=(string)trim($rcn);
		$msk_rc_fnr=$msk_rc_nr;
		if(substr($msk_rc_fnr, 0, 1) == '-')	// Maskengenerator
			$msk_rc_fnr='u'.substr($msk_rc_fnr, 1);
		$msk_rc_hidden=fieldHidden('rc_'.$msk_rc_tr.'_set',$msk_rc_nr);

		if(isset($_POST['sort_post']))
			$spval=$_POST['sort_post'];
		else
			$spval='';
		$msk_rc_hidden .= fieldHidden('sort_post',$spval);

		// Alle Reportspalten ermitteln
		if(!fxIsArray($GLOBALS['fx_mfields'][$msk_rc_nr]))
			sessionMask($msk_rc_nr);
		if(fxIsArray($GLOBALS['fx_mfields'][$msk_rc_nr]))
		{
			$msk_rc_rem=array();
			foreach($GLOBALS['fx_mfields'][$msk_rc_nr] as $f)
			{
				$valid=false;
				if(substr($msk_rc_fnr, 0, 1) == 'u')	// Maskengenerator
				{
					if(($f['anzeigeart'] == 6) || ($f['anzeigeart'] == 7) || ($f['anzeigeart'] == 8) || ($f['anzeigeart'] == 10) || ($f['anzeigeart'] == 11))
						$valid=true;
				}
				else if($f['zeile'] == 1)
					$valid=true;
				if($valid && !$msk_rc_rem[$f['feldname']])
				{
					$msk_rc_rem[$f['feldname']]=true;
					$hfn='rc_'.$msk_rc_fnr.'_'.$f['feldname'];

					if(strlen($msk_rc_fields))
						$msk_rc_fields .= '|';
					$msk_rc_fields .= $f['feldname'];

					if(isset($GLOBALS[$f['feldname'].'_ncs']))
					{
						$val=4;
//echo($f['feldname'].': $val='.$val.' -- aus <b class="s2b red">$GLOBALS['.$f['feldname'].'_ncs]</b><br />');
					}
					else if(isset($GLOBALS[$f['feldname'].'_fcs']))
					{
						$val=3;
//echo($f['feldname'].': $val='.$val.' -- aus <b class="s2b red">$GLOBALS['.$f['feldname'].'_fcs]</b><br />');
					}
					else if(isset($_POST[$hfn]) && ($f['defaultwert'] < 2))
					{
						$val=(int)$_POST[$hfn];
//echo($f['feldname'].': $val='.$val.' -- aus <b class="s2b blue">$_POST</b><br />');
					}
					else
					{
						if(substr($msk_rc_tr,0,1) == 'u')	// Maskengenerator
						{
							if(substr($f['feldname'], -5) == '_Name')
								$val=3;
							else
								$val=1;
						}
						else
							$val=(int)$f['defaultwert'];
//echo($f['feldname'].': $val='.$val.' -- aus <b class="s2b green">defaultwert</b><br />');
					}
					if($val < 3)
					{
						if($f['defaultwert'] > 1)
							$val=$f['defaultwert'];
						else if(($val > 1) && ($f['defaultwert'] < $val))
							$val -= 2;
					}

					$msk_rc_hidden	.= fieldHidden($hfn,$val);
					$msk_rc_disp	.= $val;
				}
			}
		}
	}
//echo('<pre>'.fxHtmlEncode($msk_rc_hidden).'</pre><hr>');
	echo($msk_rc_hidden.$GLOBALS['nl']);
	if(strlen($msk_rc_fields))
		echo('<div id="rcfields_'.$msk_rc_nr.'" style="display:none;">'.$msk_rc_fields.'</div>');
//echo('<pre>'.fxHtmlEncode($msk_rc_fields).'</pre><hr>');

	$msk_rc_set='rc_'.$msk_rc_tr.'_set';
	if((!isset($GLOBALS['no_columnhide']) || !$GLOBALS['no_columnhide']) && $_POST[$msk_rc_set] && (($GLOBALS['fxpglobals']['tr'] > 0) || ($GLOBALS['_masknumber'] != 466)))
	{
		// Alle Reportspalten ermitteln
		if(!fxIsArray($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']]))
			sessionMask($GLOBALS['_masknumber']);
		if(fxIsArray($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']]))
		{
			$msk_rc_rem=array();
			$mskfnr=(string)trim($GLOBALS['_masknumber']);
			if($_POST[$msk_rc_set] < 0)	// Maskengenerator
				$mskfnr='u'.substr($_POST[$msk_rc_set], 1);
			foreach($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']] as $f)
			{
				$valid=false;
				if($f['zeile'] == 1)
					$valid=true;
				if($valid && !$msk_rc_rem[$f['feldname']])
				{
					$msk_rc_rem[$f['feldname']]=true;
					if(isset($GLOBALS[$f['feldname'].'_ncs']))
						$hid=true;
					else if(isset($GLOBALS[$f['feldname'].'_fcs']))
						$hid=false;
					else
					{
						$hfn='rc_'.$mskfnr.'_'.$f['feldname'];
						$hid=false;
						if(isset($_POST[$hfn]))
						{
							if($_POST[$hfn]%2 == 0)
								$hid=true;
						}
					}
					if($hid)
					{
						$GLOBALS[$f['feldname'].'_nv']=true;
//echo('<b>Hide field:</b> '.$f['feldname'].'<br />');
					}
				}
			}
		}
	}

	// Filtericon setzen
	$maske_filtericon='maske'.$GLOBALS['_masknumber'].'_flt';
	if(isset($GLOBALS[$maske_filtericon]) && $GLOBALS[$maske_filtericon])
		$GLOBALS['mskfilter']=$GLOBALS[$maske_filtericon];
	else
		$GLOBALS['mskfilter']=0;
	$GLOBALS['gmskfilter'] |= $GLOBALS['mskfilter'];

	$maxzeilenindex=9000; // Zeile 1=Überschrift, ab Zeile 9000 beginnt der Fuss

	// Variable für den Download eines Reports
	$reportdownloadbutton=false;
	$reportheadline=$headline;

	// Waehrung aus Benutzereinstellungen ermitteln für Anzeige
	if(($GLOBALS['_masknumber'] != '44') && ($GLOBALS['_masknumber'] != '264') && ($GLOBALS['_masknumber'] != '267') && ($GLOBALS['_masknumber'] != '269'))
		$waehrung=$GLOBALS['fxpglobals']['settings']['waehrung'];

	$tc=0;

	// Prüfen, ob selbstgenerierter Report angezeigt werden soll, d.h. Maskennummer beginnt mit 'R'
	if(substr(strtoupper($GLOBALS['_masknumber']),0,1) == 'R')
	{
		$repgen=true;
		$ra=array();
		if(strlen($GLOBALS['_masknumber']) > 1)
		{
			$repid=(int)(substr($GLOBALS['_masknumber'],1));
			// Report-Name aus der Datenbank holen
			$rsql="SELECT repname, repsql FROM repliste WHERE repid=".$repid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$ra=db_values($rsql);
		}

		if(fxIsArray($ra))
		{
			$reptitle=$ra['repname'];
			$replsql=$ra['repsql'];
		}
		else
		{
			$reptitle='Report #'.$GLOBALS['_maskcounter'];
			$replsql=$GLOBALS['listensql'];
		}
		$repparse=parse_report($replsql);
//fxDebug($repparse, 'Report');

		$repsql=$repparse['sql'];
		// Reportzeilen zählen
		if($repsql)
		{
			$rcsql=$repsql;
			$frompos=strpos(strtolower($rcsql), ' from ');
			if($from !== false)
				$rcsql='SELECT COUNT(*)'.substr($rcsql, $frompos);
			$orderpos=strpos(strtolower($rcsql), ' order by ');
			if($orderpos !== false)
				$rcsql=substr($rcsql, 0, $orderpos);
			$maxcounts=(int)db_value($rcsql);
			$reptitle=maskHeadlineEntries($reptitle,true);
		}
	}
	else
		$repgen=false;

	// *************************************************************************
	// ***  Masken-Tabelle  ****************************************************
	// *************************************************************************
	$msk_filename=maske_filename($GLOBALS['_masknumber']);
	if(!file_exists($msk_filename))
		$GLOBALS['fxptdata']['marray'][$mskidname][$GLOBALS['_maskcounter']]='';

	// Seitenzähler zurücksetzen?
	if(!fxIsArray($GLOBALS['fxptdata']['marray'][$mskidname][$GLOBALS['_maskcounter']]) || ($GLOBALS['fxptdata']['marray'][$mskidname][$GLOBALS['_maskcounter']]['_masknumber'] != $GLOBALS['_masknumber']) || (isset($GLOBALS['resetpage'.$GLOBALS['_masknumber']]) && $GLOBALS['resetpage'.$GLOBALS['_masknumber']]))
		$GLOBALS['fxptdata']['marray'][$mskidname][$GLOBALS['_maskcounter']]=array('_masknumber'=>$GLOBALS['_masknumber'], 'seitenr'=>0);

	// Maskeninfos ermitteln aus Datei oder, falls noch nicht vorhanden, in Datei erstellen
	$merr=sessionMask($GLOBALS['_masknumber'], $repgen, $reptitle, $repparse);
	if($merr)
		return '';

	// und dann mit Feldnummern + Grösseninfos belegen, sowie Hidden-Fields merken
	$g=0;
	$gend=1+sizeof($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']]);
	while($g < $gend)
	{
		if($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g] == '')
		{
			$g++;
			continue;
		}
		// Spaltenbreite merken
		if(($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['spaltenanzahl'] > 1) || ($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['spaltenanzahl'] < -1))
		{
			if(abs($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['spaltenanzahl']) > (int)$GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']]['width'][$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['spalte']])
			{
				$prz='';
				if($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['spaltenanzahl'] < -20)
					$prz='%';
				$GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']]['width'][$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['spalte']]=abs($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['spaltenanzahl']).$prz;
			}
		}
		// Zeilenhöhe merken
		if(($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['zeilenanzahl'] > 1) && ($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['zeilenanzahl'] > $GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']]['height'][$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['zeile']]))
			$GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']]['height'][$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['zeile']]=$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['zeilenanzahl'];
		// Hidden-Fields merken
		if($GLOBALS['anzart_defines'][$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['anzeigeart']] == 'versteckt')
		{
			if($GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']]['hidden'] != '')
				$GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']]['hidden'] .= ','.(string)$g;
			else
				$GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']]['hidden']=(string)$g;
//echo('<B>HIDDEN:</B> $g="'.$g.'" ('.$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname'].')<br />');
		}
		// sonst Feldnummer merken
		else
		{
			if($GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['zeile']][$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['spalte']] == 'blank')
				$GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['zeile']][$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['spalte']]=(string) $g;
			else
				$GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['zeile']][$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['spalte']]=$GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['zeile']][$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['spalte']].','.$g;
		}
		$g++;
	}

	// und Ergebnis wiederum speichern
	$GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]=$GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']];
//fxDebug($GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']], __LINE__.': mfieldsinfo['.$GLOBALS['_masknumber'].']', 0);

	// NoWrap-Felder für Reportgenerator
	if(fxIsArray($repparse) && fxIsArray($repparse['fld']))
	{
		foreach($repparse['fld'] as $k => $rf)
		{
			// NoWrap
			if($rf['nowrap'])
			{
//echo('$'.$rf['rn'].'_nw=true<br />');
				$GLOBALS[$rf['rn'].'_nw']=true;
			}
		}
	}

	// Anhand des Maskenstatus prüfen, ob Maskenhilfemodus, ausser Programmierer hat Wert festgelegt
	if(isset($GLOBALS['maskenhilfemodus']))
		$mskhelpmode=$GLOBALS['maskenhilfemodus'];
	else
	{
		$mskhelpmode=false;
		if($GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['status_maske'] < 3)
			$mskhelpmode=true;
	}

	// Anzahl der Zeilen und Spalten aus Maskenfelder holen
	$anz_zeilen=0;
	if(is_array($GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]) && is_array($GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]['rows']))
		$anz_zeilen=sizeof($GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]['rows']);
	$anz_spalten=0;
	if(is_array($GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]) && is_array($GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]['columns']))
		$anz_spalten=sizeof($GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]['columns']);

	// Maskenart festlegen
	$maskenart=$GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['maskenart'];
	if(($maskenart > 1) && ($maskenart < 6)) // Variable zur Anzeige, dass es sich um eine Liste handelt
		$maskenliste=1;
	else
		$maskenliste=0;
	$GLOBALS['fxptdata']['mskislist']=$maskenliste;

	// Anzeige Array generieren
	if($mskhelpmode && fxIsArray($GLOBALS['fxptdata']['mfa_'.$GLOBALS['_masknumber']]))
		$GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']]=$GLOBALS['fxptdata']['mfa_'.$GLOBALS['_masknumber']];
	else if(fxIsArray($GLOBALS['fx_mfieldsarray']) && is_string($GLOBALS['_masknumber']) && strlen($GLOBALS['_masknumber']) && fxIsArray($GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']]))
	{
		$budgetart=strtoupper(substr($GLOBALS['fxpglobals']['settings']['budgetart'], 0, 1));
		$kst=darf_kosten_sehen();

		$GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']]=array();

		// Not_Visible Felder aus Liste streichen, ausser im Debug-Modus
		for($lnr=0; $lnr<$anz_zeilen; $lnr++)
		{
			for($cnr=0; $cnr<$anz_spalten; $cnr++)
			{
				$line=(int)$GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]['rows'][$lnr];
				$column=(int)$GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]['columns'][$cnr];
				$gex=explode(',', $GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']][$line][$column]);
				$ng='blank';
				for($gnr=0; $gnr<sizeof($gex); $gnr++)
				{
					$g=(int) $gex[$gnr];
					$gc=substr($gex[$gnr], 0, 1);
					if(is_numeric($gc))
					{
						$hide=false;

						// Ist das Feld ein int. (16), ext. (17) Budgetfeld, ein Kostenfeld oder ein Spezialfeld (Int/Ext)?
						$felder_name=strtolower($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname']);
						$felder_budget=(int)$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldtyp'];
						$felder_spezial=strtoupper($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['spezial']);
						if(isset($GLOBALS[$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname'].'_cst']))
							$felder_kosten=(int)$GLOBALS[$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname'].'_cst'];
						else if(($felder_budget == 16) || ($felder_spezial == 'INT') || (substr($felder_name, 0, 2) == 'ek'))		// Internes Kostenfeld
							$felder_kosten=1;
						else if(($felder_budget == 17) || ($felder_spezial == 'EXT') || (substr($felder_name, 0, 2) == 'vk'))	// Externes Kostenfeld
							$felder_kosten=2;
						else
							$felder_kosten=0-(int)$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['kosten'];

						if($felder_kosten != 0)
						{
							// Keine Kostenfelder anzeigen
							if(!$kst || ($budgetart == 'N'))
								$hide=true;
							else if(($kst < 3) || ($budgetart != 'B'))
							{
								// Kein speziell definiertes Kostenfeld (Int. oder Ext.)
								if($felder_kosten < 0)
								{
									if($kst < 3)
										$hide=true;
								}
								// Speziell definiertes Kostenfeld (Int. oder Ext.)
								else
								{
									// Nur internes Budget
									if(($felder_kosten == 2) && ((($kst&2) == 0) || ($budgetart == 'I')))
										$hide=true;
									// Nur externes Budget
									if(($felder_kosten == 1) && ((($kst&1) == 0) || ($budgetart == 'E')))
										$hide=true;
								}
							}
						}
//echo('<b>'.$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname'].'</b>: $budgetart='.$budgetart.', $felder_kosten='.$felder_kosten.', $kst='.$kst.', $hide='.$hide.', $not_visible='.$not_visible.'<br />');

						$not_visible=0;
						if(!$hide)
						{
							$nv_field=$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname'].'_nv';
							if(!isset($GLOBALS[$nv_field]) || ($GLOBALS[$nv_field] != 1))
							{
								if($ng === 'blank')
									$ng=$g;
								else
									$ng .= ','.$g;
							}
							else
								$not_visible=1;
						}
					}
				}
				$GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][$line][$column]=$ng;
			}
		}

		// Zeilenliste generieren, die angezeigt werden soll
		$zeilenliste='';
		for($lnr=0; $lnr<$anz_zeilen; $lnr++)
		{
			$line=(int)$GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]['rows'][$lnr];
			for($cnr=0; $cnr<$anz_spalten; $cnr++)
			{
				$column=(int) $GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]['columns'][$cnr];
				$g=(int) $GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][$line][$column];
				$gc=substr($GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][$line][$column], 0, 1);
				if(is_numeric($gc))
				{
					if($zeilenliste == '')
						$zeilenliste=$line;
					else
						$zeilenliste .= ','.$line;
					break;
				}
			}
		}
		$GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']]['lines']=$zeilenliste;

		// Spaltenliste generieren, die angezeigt werden soll
		$spaltenliste='';
		for($cnr=0; $cnr<$anz_spalten; $cnr++)
		{
			$column=(int)$GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]['columns'][$cnr];
			for($lnr=0; $lnr<$anz_zeilen; $lnr++)
			{
				$line=(int)$GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]['rows'][$lnr];
				$ltb='';
				if($line >= $maxzeilenindex)
					$ltb='*';
				$g=(int)$GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][$line][$column];
				$gc=substr($GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][$line][$column], 0, 1);
				if(is_numeric($gc))
				{
					if($spaltenliste == '')
						$spaltenliste=$column.$ltb;
					else
						$spaltenliste .= ','.$column.$ltb;
					break;
				}
			}
		}
		$GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']]['columns']=$spaltenliste;

		// Blank-Felder, die nicht angezeigt werden sollen durch 'skip' ersetzen
		for($lnr=0; $lnr<$anz_zeilen; $lnr++)
		{
			$line=(int)$GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]['rows'][$lnr];
			if($GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['listenansicht'] && ($line < $maxzeilenindex))
				continue;
			$firstvalue=false;
			for($cnr=$anz_spalten-1; $cnr>=0; --$cnr)
			{
				$column=(int)$GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]['columns'][$cnr];
				if($GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][$line][$column] === 'blank')
				{
					if($firstvalue)
					{
						$GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][$line][$column]=$GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][$line][$firstvalue];
						$GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][$line][$firstvalue]='skip';
						$firstvalue=$column;
					}
					else
						$GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][$line][$column]='skip';
				}
				else if($line < $maxzeilenindex)
					break;
				else
					$firstvalue=$column;
			}
			if($line >= $maxzeilenindex)
			{
				if(!$firstvalue)
				{
					if(isset($GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][$line]))
						unset($GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][$line]);
				}
				else
				{
					$maxcolumns='';
					foreach($GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][$line] as $ncol => $nval)
					{
						if((substr($nval, 0, 1) != 's') && (substr($nval, 0, 1) != 'b'))
							$maxcolumns .= ','.$ncol;
					}
					$GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']]['maxlines'][$line]=substr($maxcolumns, 1);
				}
			}
//echo('$line: '.$line.', $maxcolumns: '.$maxcolumns.' -> $firstvalue='.$firstvalue.'<br />');
		}
		$GLOBALS['fxptdata']['mfa_'.$GLOBALS['_masknumber']]=$GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']];
	}

	// Falls alle Felder ausgeblendet wurden, dann komplette Maske nicht anzeigen
	if(!strlen($GLOBALS['fxptdata']['mfa_'.$GLOBALS['_masknumber']]['lines']))
		return '';
//fxDebug($GLOBALS['fxptdata']['mfa_'.$GLOBALS['_masknumber']], $GLOBALS['_masknumber']);

	// Zeilen
	$lex=explode(',', $GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']]['lines']);
	if(fxIsArray($lex))
	{
		foreach($lex as $lnr => $lval)
			$lex[$lnr]=(int)$lval;
	}

	// Spalten
	$cex=explode(',', $GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']]['columns']);
	if(fxIsArray($cex))
	{
		foreach($cex as $cnr => $cval)
			$cex[$cnr]=trim($cval);
	}

	$msk_hd_str=$GLOBALS['nl'].$GLOBALS['nl'];
	$msk_bd_str='';

	// neue Tabelle erstellen
	if(strlen($GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['breite']))
	{
		if((int)$GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['breite'] < 0)
			$mwidth=abs($GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['breite']).'%';
		else
			$mwidth=$GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['breite'];
		$wmode='Mask';
	}
	else
	{
		$mwidth=940;
		$wmode='Default';
	}

	// Adjust masks if screen resolution less than mask width
	$GLOBALS['msk_wfac']=1.00;
	$swidth=$GLOBALS['fxpglobals']['swidth'];
	$siwidth=$swidth-90;
/*
	if($mwidth > $siwidth)
	{
		$GLOBALS['msk_wfac']=$siwidth/$mwidth;
		$mwidth=$siwidth;
	}
*/

	$GLOBALS['miwidth']=$mwidth;
	$GLOBALS['mowidth']=$mwidth+22;

	if($GLOBALS['fxpglobals']['tr'] == 121)	// Interner Maskentest
		echo('<div style="color:#006b9f;margin:2px;padding:4px;background:rgba(255,255,255,0.75);border:1px outset #444444;border-radius:5px;"><b>Mask:</b> swidth='.$swidth.', width mode='.$wmode.', inner width='.$GLOBALS['miwidth'].', outer width='.$GLOBALS['mowidth'].', fac='.$GLOBALS['msk_wfac'].'</div>');

	$ci='';
	if($withclose)
		$ci=closeIcon();

	$autoscroll='';
	if(isset($GLOBALS['autoscroll']) && ($GLOBALS['autoscroll'] == 'm'.$GLOBALS['_masknumber']))
		$autoscroll='<div id="autoscroll" style="float:left;"></div>';

	$msk_hd_str .= $GLOBALS['mttabs'].'<!-- DBI: Main table (start) -->'.$GLOBALS['nl'];
	$msk_hd_str .= $GLOBALS['mttabs'].$autoscroll.'<table id="mt'.$GLOBALS['_masknumber'].'_rahmen" border="0" cellpadding="0" cellspacing="0"'.$msk_align.' style="position:relative;float:none;">'.$GLOBALS['nl'];
	$msk_hd_str .= $GLOBALS['mttabs'].'	<tr id="mttr'.$maskenliste.'">'.$GLOBALS['nl'];
	$msk_hd_str .= $GLOBALS['mttabs'].'		<td align="left">'.$GLOBALS['nl'];
	$msk_hd_str .= $GLOBALS['mttabs'].'			<!-- DBI: Main div (start) -->'.$GLOBALS['nl'];
	$msk_hd_str .= $GLOBALS['mttabs'].'			<div class="maskc" style="position:relative;min-width:'.$GLOBALS['mowidth'].'px;white-space:normal;">'.$GLOBALS['nl'];

	// Titelzeile angeben, entweder vom Programm festgelegt oder aus Maske holen
	if(!$mskhelpmode || !$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']]['headline_help'])
	{
		$mskhead='maske'.$GLOBALS['_masknumber'].'_head';
		if(isset($GLOBALS[$mskhead]))
			$headline=$GLOBALS[$mskhead];
		if($headline !== 'blank')
		{
			$hilfezusatz='';
			if(substr($headline, 0, 1) == '+')
				$headline=$GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['text_kurz'].substr($headline, 1);
			else if(gettype($headline) == 'integer')
			{
				$hilfezusatz='&mzs='.$headline;
				$hl=explode('°', $GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['text_kurz']);
				$headline=$hl[$headline];
			}
			if(!strlen($headline))
				$headline=$GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['text_kurz'];
			$GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['hilfezusatz']=$hilfezusatz;

			$hai='';
			if($withclose)
				$hai='cursor:move;"'.substr(fxf_jsFunction('dragIt', '*onmousedown', 'event', 'iainfo'), 0,-1);

			$tmps='';
			$tmpe='';

			$tmps .= $GLOBALS['mttabs'].'				<!-- DBI: Header div (start) -->'.$GLOBALS['nl'];
			$tmps .= $GLOBALS['mttabs'].'				<div id="mt'.$GLOBALS['_masknumber'].'_kopf" class="maskh" style="position:relative;'.$hai.'">'.$GLOBALS['nl'];
			if(!strlen($headline) || (substr($headline,0,1) != '<'))
			{
				$tmps .= $GLOBALS['mttabs'].'					<span class="maskht">'.$GLOBALS['nl'];
				$tmpe .= $GLOBALS['mttabs'].'					</span>'.$GLOBALS['nl'];
			}
			$tmpe .= $GLOBALS['mttabs'].'				</div>'.$GLOBALS['nl'];
			$tmpe .= $GLOBALS['mttabs'].'				<!-- DBI: Header div (end) -->'.$GLOBALS['nl'];

			$tmph=$headline;
			$headline_help=str_replace('"', '&#x22;', strip_tags($tmph));
			if($GLOBALS['fxpglobals']['tr'])
				$gtr='&tr='.$GLOBALS['fxpglobals']['tr'];
			else
				$gtr='';

			// Normale Maske
			$tmpn  = $tmps;
			$tmpn .= $GLOBALS['mttabs'].'						<!--spage-->'.$tmph.'<!--epage-->'.$GLOBALS['nl'];
			$tmpn .= $GLOBALS['mttabs'].'						<div class="maski" style="position:absolute;right:0;top:0;bottom:1px;">'.$ci.$GLOBALS['nl'];
			$tmpn .= $GLOBALS['mttabs'].'							';

			$tmpl=false;

			// - Filterplatzhalter
			$tmpn .= '<!--filter-->';

			// - Checkboxplatzhalter
			$tmpn .= '<!--checkbox'.$GLOBALS['_masknumber'].'-->';

			// - Spaltenauswahl (Anzuzeigende Spalten festlegen)
			if(strlen($msk_rc_hidden) && strlen($msk_rc_nr))
			{
				$tmpn .= '<div style="float:left;margin-right:0;">';
				$tmpn .= '<img id="icrcpu_'.$msk_rc_fnr.'" class="aactive" src="'.$GLOBALS['gfxpath'].'/im_columns.png" tooltip="'.meldung(565, true).'" style="position:absolute;top:5px;left:-16px;">'.$GLOBALS['nl'];
				if(fxIsArray($GLOBALS['fx_mfields'][$msk_rc_nr]))
				{
					$fc=0;
					$def='';
					$all='';
					$ids=array();
					$idsr=array();

					foreach($GLOBALS['fx_mfields'][$msk_rc_nr] as $f)
					{
						$valid=false;
						if(substr($msk_rc_fnr, 0, 1) == 'u')	// Maskengenerator
						{
							if(($f['anzeigeart'] == 6) || ($f['anzeigeart'] == 7) || ($f['anzeigeart'] == 8) || ($f['anzeigeart'] == 10) || ($f['anzeigeart'] == 11))
								$valid=true;
						}
						else if($f['zeile'] == 1)
							$valid=true;

						if($valid)
						{
							$id=(int)$f['eingabefeld'];
							if($idsr[$id])
								continue;
							$idsr[$id]=true;

							// Default
							if(isset($GLOBALS[$f['feldname'].'_ncs']))
								$val=4;
							else if(isset($GLOBALS[$f['feldname'].'_fcs']))
								$val=3;
							else if(substr($msk_rc_fnr, 0, 1) == 'u')	// Maskengenerator
							{
								if(substr($f['feldname'], -5) == '_Name')
									$val=3;
								else
									$val=1;
							}
							else
								$val=(int)$f['defaultwert'];
							$def .= $val;

							// IDs
							$ids[$fc]=array('id'=>$id, 'def'=>$val);

							// Aktuell
							if(($val < 2) && fxIsArray($_POST))
							{
								$val=0;
								if($_POST['cb'.$msk_rc_fnr.'_'.$f['feldname']])
									$val=1;
							}

							// Alle
							if(!$val)
								$val=1;
							$all .= $val;

							$fc++;
						}
					}

					$cvasval=getColumnSelects($msk_rc_nr,$ids,$msk_rc_disp,$def,$all);
					$cva=$cvasval['cva'];
					$sval=$cvasval['sval'];
//fxDebug($cva, $sval);
					$GLOBALS['rcmain_'.$msk_rc_fnr.'_ds_array']=$cva;
					$tmpn .= fieldSelect('rcmain_'.$msk_rc_fnr,$sval,294);
					$tmpn .= '</div>'.$GLOBALS['nl'];
					echo('<div id="rcdefall_'.$msk_rc_fnr.'" style="display:none;">'.$def.'|'.$all.'</div>');
				}

				$tmpl=true;
			}

			// - Maskenscreenshot
			$mha=maskHelpFile($GLOBALS['fxpglobals']['tr'],$GLOBALS['_masknumber'],$GLOBALS['fxpglobals']['lang']);
			$mim=$GLOBALS['gfxpath'].'im_photo'.$mha['ex'].'.png';
			$tmpn .= '<img id="fxphoto_'.$GLOBALS['_masknumber'].'" src="'.$mim.'" tooltip="Take Mask Screenshot: '.$mha['id'].'" style="position:absolute;top:0;right:-36px;cursor:pointer;display:none;">';
			$tmpn .= '<a href="'.$mha['mfilepath'].'?'.$GLOBALS['datetime'].'" target="fxpmaskscreenshots" tooltip="Open Mask Screenshot: '.$mha['mfilepath'].'"><img id="fxphoto_'.$GLOBALS['_masknumber'].'_disp" src="'.$GLOBALS['gfxpath'].'im_enl.png" style="position:absolute;top:24px;right:-36px;cursor:pointer;display:none;"></a>';

			$tmpn .= $GLOBALS['nl'].$GLOBALS['mttabs'].'						</div>'.$GLOBALS['nl'];
			$tmpn .= $tmpe;

			$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']]['headline']=$tmpn;

			if(!$headline)
				$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']]['rep_headline']=$GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['text_kurz'];
			else
				$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']]['rep_headline']=$headline;
		}
	}

	$trfound=false;
	if($mskhelpmode)
		$msk_hd_str .= $GLOBALS['fx_mfields'][$GLOBALS['_masknumber']]['headline_help'];
	else
	{
		$msk_hd_str .= $GLOBALS['fx_mfields'][$GLOBALS['_masknumber']]['headline'];
		$exa=array(241=>true);
		if(isset($exa[$GLOBALS['fxpglobals']['tr']]))
			setForm($GLOBALS['_masknumber']);
		else
			setForm();
	}

	// Save headline for file manager
	$GLOBALS['fxptdata']['headline'.$GLOBALS['_masknumber']]=$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']]['rep_headline'];

	$erstezeile=0;
	$letztezeile=0;
	$erstezeilesumme=array();
	$betragssumme=array();

	// Anzahl der anzuzeigenden Einträge bestimmen
	$header=false; // Variable zur Überprüfung, ob Überschriften vorhanden sind
	$feldindex=''; // Feldindex setzen, wenn Programmierer wünscht
	$msk_index='index'.$GLOBALS['_masknumber'];
	if(isset($GLOBALS['index']) || (isset($GLOBALS[$msk_index]) && ($GLOBALS[$msk_index] == true)))
		$feldindex='['.$GLOBALS['_maskcounter'].']';
	if($maskenliste || ($GLOBALS['_maskcounter'] && isset($GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]))) // nur bei Listen
	{
		if($mskhelpmode)
		{
			$retstr=$msk_hd_str;
			if(isset($GLOBALS['fxpvars']['lookups'][$GLOBALS['_masknumber']]))
				unset($GLOBALS['fxpvars']['lookups'][$GLOBALS['_masknumber']]);
			$GLOBALS['_maskcounter']++;
			return $retstr;
		}
		else
		{
			$mrid='';
			if(fxIsArray($_POST) && $_POST['RepID'])
				$mrid='R'.$_POST['RepID'];
			$mcid='';
			if(fxIsArray($GLOBALS['fxptdata']['marray']) && fxIsArray($GLOBALS['fxptdata']['marray'][$mskidname]) && fxIsArray($GLOBALS['fxptdata']['marray'][$mskidname][$GLOBALS['_maskcounter']]))
				$mcid=$GLOBALS['fxptdata']['marray'][$mskidname][$GLOBALS['_maskcounter']]['_masknumber'];
			if(strlen($mrid) && strlen($mcid) && ($mrid === $mcid) && ($_POST['Button_Zurueck'] || $_POST['Button_Weiter']))
			{
				if($_POST['Button_Zurueck'])
					$GLOBALS['fxptdata']['marray'][$mskidname][$GLOBALS['_maskcounter']]['seitenr']--;
				else
					$GLOBALS['fxptdata']['marray'][$mskidname][$GLOBALS['_maskcounter']]['seitenr']++;
			}

			// Auf Override-Variable ???_nl prüfen, die besagt, dass alle Einträge angezeigt werden sollen
			$inputfield='maske'.$GLOBALS['_masknumber'].'_nl';
			$nolimit=false;
			if((gettype($GLOBALS[$inputfield]) == 'boolean') && ($GLOBALS[$inputfield] == true))
				$nolimit=true;

			$entries_checked == false;
			while($entries_checked == false)
			{
				// Startzeile und Endzeile berechnen
				$einst=$GLOBALS['fxpglobals']['settings']['listeneintraege'];
				if($nolimit)
					$GLOBALS['fxptdata']['marray'][$mskidname][$GLOBALS['_maskcounter']]['seitenr']=0;
				$erstezeile=$GLOBALS['fxptdata']['marray'][$mskidname][$GLOBALS['_maskcounter']]['seitenr'] * $einst;

				if(fxIsArray($GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']])) // Listenwerte wurden übergeben
				{
					$letztezeile=sizeof($GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]);
					$feldwerte=$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']];
					$einst='Size('.sizeof($feldwerte).')';
				}
				else // Normale Liste
				{
					$letztezeile=$erstezeile + $einst - 1;
					if(($maskenart == 3) || ($maskenart == 5) || $nolimit) // Gesamtlisten, d.h. alle Daten einlesen
					{
						if($repgen)
							$feldwerte=@db_values($repsql, '*');
						else
							$feldwerte=@db_values($GLOBALS['listensql'], '*');
					}
					else
					{
						if($repgen)
							$feldwerte=@db_values($repsql, $erstezeile, $letztezeile+1);
						else
							$feldwerte=@db_values($GLOBALS['listensql'], $erstezeile, $letztezeile+1);
					}
				}

				$allerletztezeile=@array_keys($feldwerte);
				if(is_null($allerletztezeile))
				{
					if($GLOBALS['fxptdata']['marray'][$mskidname][$GLOBALS['_maskcounter']]['seitenr'] > 0)
						$GLOBALS['fxptdata']['marray'][$mskidname][$GLOBALS['_maskcounter']]['seitenr']--;
					else
						$entries_checked=true;
				}
				else
					$entries_checked=true;
			}

			if($maskenart%2 == 1) // ungerade Masken: (3) Liste gesamt, (5) Report gesamt
			{
				if(is_null($allerletztezeile) || !fxIsArray($allerletztezeile))
					$allerletztezeile=-1;
				else
					$allerletztezeile=$allerletztezeile[sizeof($allerletztezeile) - 1];
				$letztezeile=$allerletztezeile;
			}
			else if(!is_null($allerletztezeile))
			{
				$allerletztezeile=$allerletztezeile[sizeof($allerletztezeile) - 1];
				if($nolimit)
					$letztezeile=$allerletztezeile;
				else if($letztezeile > $allerletztezeile)
					$letztezeile=$allerletztezeile;
			}
			else
			{
				$allerletztezeile=-1;
				$letztezeile=-1;
			}

			// Falls keine Einträge in Datenbank -> Anwender benachrichtigen
			if(is_null($allerletztezeile) || ($allerletztezeile < 0))
			{
				$msk_bd_str0 .= maskNoEntries($GLOBALS['_masknumber'], -1, true);
				$msk_bd_str0 .= shutdown_mask($GLOBALS['_masknumber']);

				$msk_bd_str .= $msk_bd_str0;

				// Eintrag anzeigen und internen Masken-Zähler erhöhen
				if(isset($GLOBALS['fxpvars']['lookups'][$GLOBALS['_masknumber']]))
					unset($GLOBALS['fxpvars']['lookups'][$GLOBALS['_masknumber']]);
				$GLOBALS['_maskcounter']++;
				return $msk_hd_str.$msk_bd_str;
			}

			// Im Reportmodus, falls Einträge gefunden wurden, dann Download-Button anzeigen
			if(($maskenart > 3) && ($maskenart < 6) && ($GLOBALS['_masknumber'] != '403') && !isset($_GET['no_rdb']) && (!isset($GLOBALS['no_rdb']) || !$GLOBALS['no_rdb']) && (!isset($GLOBALS['no_dmanager']) || !$GLOBALS['no_dmanager']))
				$reportdownloadbutton=true;
		}

		// Innere Tabelle öffnen
		$msk_bd_str .= $GLOBALS['mttabs'].'					<!-- DBI: Body div (start) -->'.$GLOBALS['nl'];
		$msk_bd_str .= $GLOBALS['mttabs'].'					<div class="maskm" style="position:relative;white-space:normal;">'.$GLOBALS['nl'];
		$msk_bd_str .= $GLOBALS['bttabs'].open_table($GLOBALS['_masknumber'], $GLOBALS['fx_mask'][$GLOBALS['_masknumber']]).$GLOBALS['nl'];

		$summenspalten='';

		// Gat max columns
		$GLOBALS['columncountermax']=0;
		if($maskenliste)
		{
			foreach($cex as $cnr => $column)
			{
				if(strpos($column, '*') > 0)
					continue;
				$column=(int)$column;

				if($GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][1][$column] === 'skip')
					break;
				if(($GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][1][$column] === 'blank') && !$maskenliste && !$GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['listenansicht'])
				{
					$cnr++;
					continue;
				}

				$GLOBALS['columncountermax']=max($GLOBALS['columncountermax'], $cnr);
			}
		}
//echo('$GLOBALS[\'columncountermax\']='.$GLOBALS['columncountermax'].'<hr />');

		// Alle Überschriften der ersten Zeile ausgeben
		if($maskenliste && ($GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]['rows'][0] == 1) && !$maske_noheadlines)
		{
			$header=true;
			if($GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['listenansicht'])
			{
				$GLOBALS['listheaderc']++;
				$tmp=$GLOBALS['bttabs'].'	<tr id="listheader'.$GLOBALS['listheaderc'].'">'.$GLOBALS['nl'];
			}
			else
			{
				$tmp  = $GLOBALS['bttabs'].'	<tr>'.$GLOBALS['nl'];
				$tmp .= $GLOBALS['bttabs'].'		'.tdo().'>'.$GLOBALS['nl'];
				$tmp .= $GLOBALS['bttabs'].'			<table border="0" cellpadding="0" cellspacing="0">'.$GLOBALS['nl'];
				$tmp .= $GLOBALS['bttabs'].'				<tr>'.$GLOBALS['nl'];
			}

			$GLOBALS['columncounter']=0;
			$GLOBALS['columncountermax']=0;
			$GLOBALS['columnspacer']=false;

			foreach($cex as $cnr => $column)
			{
				if(strpos($column, '*') > 0)
					continue;
				$column=(int)$column;

				if($GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][1][$column] === 'skip')
					break;
				if(($GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][1][$column] === 'blank') && !$maskenliste && !$GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['listenansicht'])
				{
					$cnr++;
					continue;
				}

				$GLOBALS['columncounter']=$cnr;

				$gex=explode(',', $GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][1][$column]);
				$gnr=0;
				$gsize=sizeof($gex);
				while($gnr < $gsize)
				{
					$g=(int)$gex[$gnr];
					$gfn='';
					if(!$GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['listenansicht'])
					{
						// Maximale Spaltengröße ermitteln
						$width=0;
						for($gnr2=0; $gnr2<$gsize; $gnr2++)
						{
							$g2=(int)$gex[$gnr2];
							$gfn=$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g2]['feldname'];
							if(isset($GLOBALS[$gfn.'_twidth']))
								$twidth=(int)$GLOBALS[$gfn.'_twidth'];
							else if($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g2]['spaltenanzahl'] > 1)
								$twidth=(int)$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g2]['spaltenanzahl'];
							else if($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g2]['spaltenanzahl'] < -20)
								$twidth=abs($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g2]['spaltenanzahl']).'%';
							else
								$twidth=0;
							$width=max($twidth, $width);
						}
					}
					else if(isset($GLOBALS['column'.$column.'_twidth']))
						$width=(int)$GLOBALS['column'.$column.'_twidth'];
					else
						$width=$GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]['width'][$column];
//echo('[1] $column='.$column.', $gfn='.$gfn.' -> $width='.$width.'<br />');

					if(($gex[$gnr] === 'blank') || (substr(strtolower($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname']), 0, 5) == 'dummy') || ($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['anzeigegroesse1'] == -1)) // leerer Eintrag
					{
						$tmb=$GLOBALS['bttabs'].'		'.tdo();
						if($width)
						{
							if(substr($width, -1) == '%')
								$tmb .= ' width="'.$width.'"';
							else
								$tmb .= ' width="'.(int)($width*$GLOBALS['msk_wfac']).'"';
						}
						$tmb .= getColumnspace('blank',1,true,true,$width).'>&nbsp;';
						$tmp .= $tmb;
					}
					else
					{
						$showlit='';
						$mfld=$GLOBALS['anzart_defines'][(int)$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['anzeigeart']];
						if($mfld == 'summe')
							$summenspalten .= '|+'.substr('00000'.(int)$cnr, -5);

						// Bei Listen Überschriften anzeigen
						if((substr($mfld, 0, 7) != 'literal') && ($maskenart > 1) && ($maskenart < 6))
						{
							if($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['anzeigegroesse1'])
								$showlit=$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['anzeigegroesse1'];
							else
							{
								$showlit=1 + (int)((int)$width/80);
								$showlit=min(4, max($showlit, 1));
							}
						}

						// Bei Literalen evtl. Tooltips anzeigen
						$fld_tooltip=getFieldTooltip($GLOBALS['_masknumber'],$g);
//echo('<pre style="color:#e1001a;">'.fxHtmlEncode($fld_tooltip).'</pre>');
						if($gnr == 0)
						{
							$hdc='rhd';
							if(isset($GLOBALS[$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname'].'_litclass']))
								$hdc=$GLOBALS[$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname'].'_litclass'];

							$add='';
							if($GLOBALS['fxpglobals']['tr'] < 0)
								$add=' onmouseover="$(\'sort_'.$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname'].'\').style.display=\'\';" onmouseout="$(\'sort_'.$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname'].'\').style.display=\'none\';"';

							$tmp .= $GLOBALS['bttabs'].'		'.get_columninfo($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g], $aktzeile, $hdc, $width, 'header', $msk_nowrap, $fld_tooltip, $add).$GLOBALS['nl'].$GLOBALS['bttabs'].'		';
						}
						else
							$tmp .= '	<br />'.$GLOBALS['nl'].$GLOBALS['bttabs'].'		';

						if(strlen($fld_tooltip))
							$stt=-1;
						else
							$stt=0;

						$columnheader='<div style="position:relative;">';
						$css=$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname'].'_sort';
						if(isset($GLOBALS[$css]))
						{
							$so=strtolower(substr($GLOBALS[$css],0,1));	// a=ASC, d=DESC
							if($GLOBALS['fxpglobals']['lang'] == 1)
							{
								if($so == 'a')
									$stt='Aufsteigend sortiert: A &darr; Z / 0 &darr; 9';
								else
									$stt='Absteigend sortiert: Z &darr; A / 9 &darr; 0';
							}
							else if($so == 'a')
								$stt='Sorted in ascending order: A &darr; Z / 0 &darr; 9';
							else
								$stt='Sorted in descending order: Z &darr; A / 9 &darr; 0';
							$columnheader .= '<img src="'.$GLOBALS['gfxpath'].'srt'.$so.'.png" tooltip="'.$stt.'" style="position:absolute;right:0;cursor:help;">';
						}
						else if($GLOBALS['fxpglobals']['tr'] < 0)
						{
							$columnheader .= '<span style="position:relative;">';
							$si='';
							if(isset($_POST['sort_post']) && strlen($_POST['sort_post']) && (substr($_POST['sort_post'], 1) == $GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname']))
							{
								$si=substr($_POST['sort_post'], 0, 1);
								$columnheader .= '<img src="'.$GLOBALS['gfxpath'].'sort_'.$si.'_full.png" align="middle">&nbsp;';
							}
						}
						$headertext=get_array_entry($GLOBALS['_masknumber'], '', $GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g], false, '', $GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['status_maske'], $showlit, $stt, 'header');
						$columnheader .= '<span style="position:relative;">'.$headertext.'</span>';
						if(isset($GLOBALS[$css]))
							$columnheader .= '&nbsp;&nbsp;&nbsp;';
						else if($GLOBALS['fxpglobals']['tr'] < 0)
						{
							$columnheader .= '</div>';

							if($GLOBALS['fxpglobals']['lang'] == 1)
							{
								$ttu='Aufsteigend sortieren: A&gt;Z';
								$ttd='Absteigend sortieren: Z&gt;A';
							}
							else
							{
								$ttu='Sort in ascending order: A&gt;Z';
								$ttd='Sort in descending order: Z&gt;A';
							}
							$sh=8;
							$columnheader .= '<div id="sort_'.$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname'].'" style="position:relative;text-align:right;height:'.$sh.'px;float:right;border:1px solid #cccccc;background:#d8d8d8;margin:-13px 0 0 0;padding:3px 0;overflow:hidden;display:none;">&nbsp;<img src="'.$GLOBALS['gfxpath'].'sort_u_disp.png" align="top" style="margin:0;padding:0;cursor:pointer;" '.fxf_jsFunction('sortColumn', 'u'.$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname']).' tooltip="'.$ttu.'">&nbsp;<img src="'.$GLOBALS['gfxpath'].'sort_d_disp.png" align="top" style="margin:0;padding:0;cursor:pointer;" '.fxf_jsFunction('sortColumn', 'd'.$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname']).' tooltip="'.$ttd.'">&nbsp;</div>';
						}
						$columnheader .= '</div>';
//echo('<pre>'.fxHtmlEncode($columnheader).'</pre><pre>'.fxHtmlEncode($headertext).'</pre>');

						$GLOBALS['fx_mheaders'][$GLOBALS['_masknumber']][$g]=$headertext;
						$tmp .= '	'.$columnheader.$GLOBALS['nl'].$GLOBALS['bttabs'].'		';
					}
					$gnr++;
				}
				$tmp .= tdc(true);
				$cnr++;
			}
			$tmp .= $GLOBALS['bttabs'].'	'.trc(true);
			if(!$GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['listenansicht'])
			{
				$tmp .= $GLOBALS['bttabs'].'			</table>'.$GLOBALS['nl'];
				$tmp .= $GLOBALS['bttabs'].'		'.tdc(true);
			}
			$msk_bd_str .= $tmp;
//echo('<pre>'.fxHtmlEncode($tmp).'</pre>');
		}
	}
	else // bei normaler Maske nun die Innere Tabelle öffnen
	{
		$msk_bd_str .= $GLOBALS['mttabs'].'					<!-- DBI: Body div (start) -->'.$GLOBALS['nl'];
		$msk_bd_str .= $GLOBALS['mttabs'].'					<div class="maskm" style="position:relative;white-space:normal;">'.$GLOBALS['nl'];
		$msk_bd_str .= $GLOBALS['bttabs'].open_table($GLOBALS['_masknumber'], $GLOBALS['fx_mask'][$GLOBALS['_masknumber']]).$GLOBALS['nl'];
	}
//echo('<pre>'.fxHtmlEncode($msk_bd_str).'</pre>');

	$GLOBALS['bodyzeilenindex']='';

	// und jetzt den restlichen Body generieren
	$summe=false;
	$Buttonleiste=false;
	$inner_table_closed=false;
	for($aktzeile=$erstezeile; $aktzeile<=$letztezeile; $aktzeile++)
	{
		if($maskenliste && !$feldwerte[$aktzeile])
			continue;

		$tmp='';
		$hidden=false;

		$GLOBALS['bodyzeilenindex']='';
		if($feldindex != '')
			$GLOBALS['bodyzeilenindex']=$feldindex.'['.(string) $aktzeile.']';

		if($maskenart == 2)	// Liste einzeln
			setForm('',$aktzeile);

		foreach($lex as $lnr => $line)
		{
			if(($line == 1) && $header && ($maskenart < 4)) // falls Überschriften vorhanden, dann mit Zeile 1 anfangen, ausser bei Report
				continue;

			if($line >= $maxzeilenindex) // keine allgemeingültige Zeilen
			{
				if($aktzeile < $letztezeile)
					break;

				$bcex=explode(',', $GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']]['maxlines'][$line]);
				if(fxIsArray($bcex))
				{
					foreach($bcex as $cnr => $cval)
						$bcex[$cnr]=trim($cval);
				}
			}
			else
				$bcex=$cex;

			$GLOBALS['tropen']=0;
			$GLOBALS['columncounter']=0;
			$GLOBALS['columnspacer']=false;

			foreach($bcex as $cnr => $column)
			{
				if((strpos($column, '*') > 0) && ($line < $maxzeilenindex))
					continue;

				$column=(int)$column;

				if($GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][$line][$column] === 'skip')
					break;
				if(($GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][$line][$column] === 'blank') && !$maskenliste && !$GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['listenansicht'])
					continue;
				// Blank-Einträge in Zeilen mit # >= 9000 werden ignoriert
				if(($GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][$line][$column] === 'blank') && ($GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]['rows'][$lnr] >= $maxzeilenindex))
					continue;

				if($maskenliste)
					$GLOBALS['columncounter']=$cnr;
				if($GLOBALS['tropen'] == 0) // tr-tag setzen
				{
					$tmpz='';
					$trin='';
					if($line >= $maxzeilenindex) // keine allgemeingültige Zeilen
					{
						$inner_table_closed=true;
						if(!$Buttonleiste)
						{
							$tmpz .= $GLOBALS['ittabs'].'</table>'.$GLOBALS['nl'];
							$GLOBALS['ittabs']=substr($GLOBALS['ittabs'],0,-3);
						}
						if(!$body_closed)
						{
							$tmpz .= $GLOBALS['mttabs'].'					</div>'.$GLOBALS['nl'];
							$tmpz .= $GLOBALS['mttabs'].'					<!-- DBI: Body div (end) -->'.$GLOBALS['nl'];
							$body_closed=true;
						}

						if(!$Buttonleiste)
						{
							$btncnt=0;
							$bascnt=0;
							$ftr=false;
							if(fxIsArray($_GET) && isset($_GET['pname']) && (substr($_GET['pname'],-4) == '.png'))
								$ftr=true;
							foreach($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']] as $mk => $mc)
							{
								if(fxIsArray($mc) && isset($mc['zeile']))
								{
									$mcl=(int)$mc['zeile'];
									if($mcl >= $maxzeilenindex)
									{
										$btncnt++;
										if($GLOBALS['check_baseline'] && ($mc['vertausr'] == 'baseline') && !$ftr)
											$bascnt++;
									}
								}
							}
							if($btncnt == $bascnt)
								$Buttondisp='none';
							else
								$Buttondisp='block';

							$tmpz .= $GLOBALS['mttabs'].'					<!-- DBI: Button div (start) -->'.$GLOBALS['nl'];
							$tmpz .= $GLOBALS['mttabs'].'					<div class="maskb" style="position:relative;display:'.$Buttondisp.';">'.$GLOBALS['nl'];

							$Buttonleiste=true;
						}
					}
					else if($GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]['height'][$line] > 10)
						$trin .= ' height="'.$GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]['height'][$line].'"';

//echo('maske: $GLOBALS[\'trinfo'.$GLOBALS['_maskcounter'].'_'.$aktzeile.'\']='.$GLOBALS['trinfo'.$GLOBALS['_maskcounter'].'_'.$aktzeile].'<br />');
					if(isset($GLOBALS['trinfo'.$GLOBALS['_maskcounter'].'_'.$aktzeile]))
						$trin .= ' '.trim($GLOBALS['trinfo'.$GLOBALS['_maskcounter'].'_'.$aktzeile]);

					if($line < $maxzeilenindex)
					{
						$tmpz .= $GLOBALS['ittabs'].'	'.tro($aktzeile).$trin.'>'.$GLOBALS['nl'];
						if(!$GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['listenansicht'])
						{
							$tmpz .= $GLOBALS['ittabs'].'		'.tdo().'>'.$GLOBALS['nl'];
							$GLOBALS['ittabs']=$GLOBALS['ittabs'].'			';
							$tmpz .= $GLOBALS['ittabs'].'<!-- DBI: Inner table (start) -->'.$GLOBALS['nl'];
							$tmpz .= $GLOBALS['ittabs'].'<table id="mt'.$GLOBALS['_masknumber'].'_z'.$line.'" border="0" cellpadding="0" cellspacing="0">'.$GLOBALS['nl'];
							$tmpz .= $GLOBALS['ittabs'].'	'.tro($aktzeile,true);
						}
						$GLOBALS['tdopen']=0;
					}
					$tmp .= $tmpz;
				}

				if($aktzeile == $erstezeile) // Betrag zu Beginn löschen
				{
					$erstezeilesumme[$cnr]=0.0;
					$betragssumme[$cnr]=0.0;
				}

				$gex=explode(',', $GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][$line][$column]);
				foreach($gex as $gnr => $g)
				{
					$g=(int)$g;
					$feldname=$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname'];
//echo($g.': $feldname='.$feldname.'<br />');
					if(!$GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['listenansicht'] || ($line >= $maxzeilenindex))
					{
						if(isset($GLOBALS[$feldname.'_twidth']))
							$width=(int)$GLOBALS[$gfn.'_twidth'];
						else if($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['spaltenanzahl'] > 1)
							$width=(int) ($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['spaltenanzahl']);
						else if($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['spaltenanzahl'] < -20)
							$width=abs($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['spaltenanzahl']).'%';
						else
							$width=0;
					}
					else if(isset($GLOBALS['column'.$column.'_twidth']))
						$width=(int)$GLOBALS['column'.$column.'_twidth'];
					else
						$width=$GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]['width'][$column];
					$field_nowidth=$feldname.'_nowidth';
					if(isset($GLOBALS[$field_nowidth]) && ($GLOBALS[$field_nowidth] === true))
						$width=0;
//echo('[2] $column='.$column.', $feldname='.$feldname.' -> $width='.$width.'<br />');

					// Darstellungsart festlegen
					$mfld=$GLOBALS['anzart_defines'][(int) $GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['anzeigeart']];

					$field_nowrap='';
					if(!$msk_nowrap)
					{
						$mftp=(int)$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldtyp'];
						if(($mftp == 5) || ($mftp == 6) || ($mftp == 7) || ($mftp == 9) || ($mftp == 10) || ($mftp == 11) || ($mftp == 12) || ($mftp == 14))
							$field_nowrap=' nowrap';
						else
						{
							$field_nowrap=$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname'].'_nw';
							if(isset($GLOBALS[$field_nowrap]) && ($GLOBALS[$field_nowrap] === true))
								$field_nowrap=' nowrap';
							else
								$field_nowrap='';
						}
					}

					$tdwaehrung='';
					$lower_feldname=strtolower($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname']);
					if(!$GLOBALS['no_waehrung'] && (substr($mfld, 0, 7) != 'literal') && ($mfld != 'bild') && ($mfld != 'icon') && ($mfld != 'checkbox') && (substr($lower_feldname, 0, 5) != 'dummy')) // Währung nur falls kein Literal, kein Dummyfeld und nicht ausgeschaltet
					{
						// Überschreiben
						if(isset($GLOBALS[$lower_feldname.'_unit']) && is_string($GLOBALS[$lower_feldname.'_unit']) && strlen($GLOBALS[$lower_feldname.'_unit']))
							$tdwaehrung=$GLOBALS[$lower_feldname.'_unit'];
						// Betragsfeld (in Firmenwährung)
						else if(($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldtyp'] == 11) || ($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldtyp'] == 16) || ($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldtyp'] == 17))
							$tdwaehrung=$waehrung;
						// Aufwandsfeld in Reports (in Stunden)
						else if((strtoupper(substr($GLOBALS['_masknumber'], 0, 1)) == 'R') && (substr($lower_feldname, 0, 7) == 'aufwand'))
							$tdwaehrung='h';
					}
					$field_nounit=$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname'].'_nu';
					if(isset($GLOBALS[$field_nounit]) && ($GLOBALS[$field_nounit] === true))
					{
						$tdwaehrung='';
						$GLOBALS['fxptdata']['nounits_'.$GLOBALS['_masknumber']][$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname']]=true;
//fxDebug($GLOBALS['fxptdata']['nounits_'.$GLOBALS['_masknumber']], 'nounits_'.$GLOBALS['_masknumber']);
					}

					if($GLOBALS['_maskcounter'] && isset($GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]))
						$feldwert=$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$aktzeile][$lower_feldname];
					else if(fxIsArray($maskenfeldwerte))
						$feldwert=$maskenfeldwerte[$lower_feldname];
					else
						$feldwert='';

					$fld_tooltip='';
					$feldwert_override='';
					$feldwert_override_set=false;

					$zw_summe=false;
					if(is_string($feldwert))
					{
						if(substr($feldwert, 0,2) == '~#')	// Tooltip
						{
							$pipepos=strpos($feldwert, '|');
							if($pipepos === false)
							{
								$fld_tooltip=substr($feldwert, 2);
								$feldwert='';
							}
							else
							{
								$fld_tooltip=substr($feldwert, 2, $pipepos-2);
								$feldwert=substr($feldwert, $pipepos+1);

								$feldwert_override=$feldwert;
								$feldwert_override_set=true;
							}
						}

						if(substr($feldwert,0,4) == '#nu#')
						{
							$feldwert=substr($feldwert,4);
							$tdwaehrung='';
						}
						if($feldwert == '~')
							$tdwaehrung='';
						if((substr($feldwert, 0, 2) == '~|') || (substr($feldwert, 0, 4) == '~-->'))
						{
							$zw_summe=true;
							$tdwaehrung='';
						}
					}

					if($zw_summe || ($gex[$gnr] === 'blank') || (substr($lower_feldname, 0, 5) == 'dummy') || ($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['anzeigegroesse1'] == -1)) // leerer Eintrag
					{
						$tmpz='';
						if($GLOBALS['tdopen'] > 0)
							$tmpz .= close_td($tdwaehrung, 'col'.$showid);
						$s_valign='';
						if(!empty($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['vertausr']) && (strtolower($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['vertausr']) != 'middle'))
							$s_valign=' valign="'.$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['vertausr'].'"';
						$s_width='';
						if($width !== 0)
							$s_width=' width="'.(int)($width*$GLOBALS['msk_wfac']).'"';
						$tmpz .= '					'.tdo().$s_width.$msk_nowrap.$field_nowrap.$s_valign.getColumnspace($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g],$aktzeile,false,true,$width).'>';
						if(is_string($feldwert) && (substr($feldwert,0,1) == '~') && (substr($feldwert,1,1) != '|'))
							$tmpz .= substr($feldwert, 1);
						else
							$tmpz .= '&nbsp;';
						$tmp .= $tmpz;
					}
					else
					{
						if($GLOBALS['tdopen'] == 0)
						{
//echo('<B>Maskenfeld:</B> "'.$mfld.'"<br />');
							$special=strtolower(trim($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['spezial']));
							$len_mfld=strlen($mfld);
							$ppstart=0;
							if(is_string($feldwert))
							{
								if(substr($feldwert, 0, 8+$len_mfld) == '^class^'.$mfld.'_')	// Prüfen, ob Farb-Override "Class"
								{
									$ppstart=8+$len_mfld;
									$ppsign='^';
								}
								else if(substr($feldwert, 0, 5+$len_mfld) == '^id^'.$mfld.'_')	// Prüfen, ob Farb-Override "ID"
								{
									$ppstart=5+$len_mfld;
									$ppsign='';
								}
								else if(substr($feldwert, 0, 6+$len_mfld) == '^col^'.$mfld.'_')	// Prüfen, ob Farb-Override "COLOR"
								{
									$ppstart=6+$len_mfld;
									$ppsign='#';
								}
								else if(substr($feldwert, 0, 7) == '^class_')					// Prüfen, ob Farb-Override "Class"
								{
									$ppstart=7;
									$ppsign='^';
								}
								else if(substr($feldwert, 0, 4) == '^id_')						// Prüfen, ob Farb-Override "ID"
								{
									$ppstart=4;
									$ppsign='';
								}
								else if(substr($feldwert, 0, 5) == '^col_')						// Prüfen, ob Farb-Override "COLOR"
								{
									$ppstart=5;
									$ppsign='#';
								}
							}

							if($ppstart)													// Farb-Override
							{
								$pipepos=strpos($feldwert, '|', $ppstart);
								if($pipepos)
								{
									$ppcol=substr($feldwert, $ppstart, $pipepos-$ppstart);
									if(substr($ppcol, 0, 1) == '?')
									{
										if($special !== 'x')
											$showid='rv'.(string)(1 + $aktzeile%2).substr($ppcol, 1);
									}
									else if(substr($ppcol, 0, 1) == '*')
										$showid='transparent'.substr($ppcol, 1);
									else
										$showid=$ppsign.$ppcol;
									$feldwert_override=substr($feldwert, $pipepos+1);
									$feldwert_override_set=true;
								}
							}
							else if($maskenart < 4)
							{
								$changeinput=false;

								$inputfield='maske'.$GLOBALS['_masknumber'].'_sf';
								$changemaske=false;
								if(isset($GLOBALS[$inputfield]) && ($GLOBALS[$inputfield] === true))
									$changemaske=true;
								else if(isset($GLOBALS[$inputfield]) && ($GLOBALS[$inputfield] === 1))	// Alle Felder auf "DISABLED" setzen
									$nva=true;
								else if(isset($GLOBALS[$inputfield]) && ($GLOBALS[$inputfield] == 2))	// Gesamte Maske im Anzeige-Modus
								{
									$changemaske=true;
									$inputfield=$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname'].'_sf';
									if(!isset($GLOBALS[$inputfield]) || (gettype($GLOBALS[$inputfield]) != 'integer'))
										$changeinput=true;
								}

								if(!$changemaske)
								{
									$inputfield=$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname'].'_sf';
									$changefield=0;
									if((!isset($GLOBALS[$inputfield]) || ($GLOBALS[$inputfield] === false)) && (($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE)))
										$changefield=1;
									else if(isset($GLOBALS[$inputfield]) && (gettype($GLOBALS[$inputfield]) == 'integer'))
										$changefield=$GLOBALS[$inputfield];

									if($changefield)
										$changeinput=true;
								}
								$showid='';
								if($special !== 'x')
								{
									if((substr($mfld, 0, 8) == 'textfeld') || (($mfld == 'checkbox') && ($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['spalte'] > 1)) || ($mfld == 'auswahlfeld'))
									{
										// $showid='repheader';
										if($changeinput || ($maskenart > 1))
										{
											if($maskenart > 1)
												$showid='rv'.(string) (1 + $aktzeile%2);
											else
												$showid='ro';

											if((!isset($GLOBALS[$field_nowidth]) || ($GLOBALS[$field_nowidth] !== true)) && ($width === 0) && ($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['anzeigegroesse1'] > 0))
												$width=$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['anzeigegroesse1']*7;
										}
									}
									else if($mfld == 'nur lesen-textfeld')
									{
										if($maskenart > 1)
											$showid='rv'.(string) (1 + $aktzeile%2);
										else
											$showid='ro';
									}
								}
							}
							else if(is_string($feldwert) && substr($feldwert, -2) == '|z') // Zwischensumme
								$showid='bcr1';
							else if($special !== 'x') // Report
								$showid='rv'.(string) (1 + $aktzeile%2);

							// Nur-Anzeige-Checkbox
							if(($showid == 'ro') && ($mfld == 'checkbox'))
								$showid='';

							if($gnr == 0)
							{
								$tmpz='';
								if($GLOBALS['tdopen'] > 0)
									$tmpz .= $GLOBALS['ittabs'].'		'.close_td($tdwaehrung, 'col'.$showid);
								if($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['zeile'] < $maxzeilenindex)
								{
									$tmpz .= $GLOBALS['ittabs'].'		';
									if(isset($ganz) && ($ganz > 1))
									{
										$tmpz .= tdo().$msk_nowrap.$field_nowrap.'>'.$GLOBALS['nl'];
										$GLOBALS['ittabs'] .= '			';
										$tmpz .= $GLOBALS['ittabs'].'<table border="0" cellpadding="0" cellspacing="0">'.$GLOBALS['nl'];
										$tmpz .= $GLOBALS['ittabs'].'	'.tro($aktzeile,true);
										$tmpz .= $GLOBALS['ittabs'].'		';
									}
									$tmpz .= get_columninfo($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g], $aktzeile, $showid, $width, 'left', $msk_nowrap.$field_nowrap, $fld_tooltip).$GLOBALS['nl'];
								}
								else
									$tmpz .= $GLOBALS['ittabs'].'		';
								$tmp .= $tmpz;
							}
							else
							{
								$tmpz=tdc(true);
								$tmpz .= trc(true);
								$tmpz .= tro($aktzeile,true);
								$tmpz .= get_columninfo($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g], $aktzeile, $showid, $width, 'left', $msk_nowrap.$field_nowrap, $fld_tooltip).$GLOBALS['nl'];
								$tmp .= $tmpz;
							}
						}
						if($tdwaehrung != '')
						{
							$tmpz  = '<table class="col'.$showid.'" border="0" cellpadding="0" cellspacing="0">'.$GLOBALS['nl'];
							$tmpz .= tro($aktzeile,true);
							$tmpz .= tdo().' class="col'.$showid.'"'.$msk_nowrap.$field_nowrap.'>'.$GLOBALS['nl'];
							$tmp .= $tmpz;
						}

						// Anzeigewerte ermitteln
						if($feldwert_override_set) // Override-Variable
							$feldwert=$feldwert_override;
						else if($maskenart == 1) // normale Maske
						{
							if((substr($mfld, 0, 7) == 'literal'))
								$feldwert=':';
							else if($GLOBALS['_maskcounter'] && isset($GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]))
								$feldwert=$feldwerte[$aktzeile][$lower_feldname];
							else if(fxIsArray($maskenfeldwerte))
							{
								$feldwert=$maskenfeldwerte[$lower_feldname];
								if($GLOBALS['uid'] === 'PReload')
								{
//echo('$uid=<b>'.$GLOBALS['uid'].'</b>, $form=<b>'.$GLOBALS['_form'].'</b> -- <b class=blue>'.$feldname.'</b>: $feldwert=&quot;<b>'.$feldwert.'</b>&quot;<br />');
									if(fxIsArray($GLOBALS['fxptarray']) && fxIsArray($GLOBALS['fxptarray'][$GLOBALS['_form']]) && isset($GLOBALS['fxptarray'][$GLOBALS['_form']][$feldname]))
									{
//echo('- $fxptarray=&quot;<b>'.$GLOBALS['fxptarray'][$GLOBALS['_form']][$feldname].'</b>&quot;<br />');
										$feldwert=$GLOBALS['fxptarray'][$GLOBALS['_form']][$feldname];
									}
									else if(fxIsArray($_POST) && isset($_POST[$feldname]))
									{
//echo('- $_POST=&quot;<b>'.$_POST[$feldname].'</b>&quot;<br />');
										$feldwert=$_POST[$feldname];
									}
//echo('-&gt; $feldwert=&quot;<b>'.$feldwert.'</b><hr />');
								}
							}
							else
								$feldwert='';
						}
						else
							$feldwert=$feldwerte[$aktzeile][$lower_feldname];
						// Ist der Feldwert ein Array (z.B. Multi-Selectfeld) => kein trim()
						if(!is_array($feldwert))
							$feldwert=trim($feldwert);

						$tilde=0;
						if(is_string($feldwert) && substr($feldwert, -2) == '|z')
							$feldwert=substr($feldwert, 0, -2);
						if(is_string($feldwert) && substr($feldwert, 0, 1) == '~') // Feld ohne Überprüfung übernehmen
						{
							if(substr($feldwert, 1, 1) == '~') // jedoch als HIDDEN-Field mitgeben
							{
								$fwert=substr($feldwert, 2);
								if($fwert == '^')
								{
									$tilde=1;
									$fwert='';
								}

								$tmp2=fieldHidden($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname'].$GLOBALS['bodyzeilenindex'],$fwert);
							}
							else
							{
								$fwert=substr($feldwert, 1);
								if($fwert == '^')
								{
									$tilde=1;
									$fwert='';
								}
								$tmp2='';
							}

							$tmp_mhz=strip_help_tags(trim($fwert));
							if($nva)
								$tmp .= $tmp_mhz;
							else
								$tmp .= $fwert.$tmp2;
						}
						else
						{
							if($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['nachschlage'] && ($mfld != 'auswahlfeld') && ($mfld != 'checkbox')) // Nachschlagewert umwandeln
							{
								if(substr($feldwert,0,1) == '^')
									$feldwert=substr($feldwert,1);
								else if($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['repeval'] && $refw[$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['repeval']])
								{
									$retr_table=array('&quot;' => '"', '$var' => $feldwert);
									if(is_string($feldwert) && strlen($feldwert))
									{
										$repeval=strtr($refw[$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['repeval']], $retr_table);
										eval($repeval);
									}
								}
								else if(($maskenart < 4) || ($maskenart > 5)) // Kein Report
								{
									$mld=@meldung($feldwert, true);
									if($mld)
										$feldwert=get_entry($mld);
								}
							}
							// und Feld generieren
							if($maskenliste) // Listen
							{
								if(!$mskhelpmode)
								{
									if(($maskenart > 3) && ($maskenart < 6)) // Report
									{
										if(is_string($feldwert) && substr($feldwert, -1) == '^')
											$fw=substr($feldwert, 0, -1);
										else
											$fw=$feldwert;
										$zahl=get_db_value($lower_feldname, $fw);
										if($zahl === false)
											$zahl=get_entry($fw, $GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]);

										if(substr($zahl, 0, 1) == '-')
										{
											if($GLOBALS['negcol'])
												$col='color="'.$GLOBALS['negcol'].'"';
											else
												$col='class="red"';
											$tmpz='<font '.$col.'>'.$zahl.'</font>';
											$GLOBALS['negnumber']=true;
										}
										else
										{
											$tmpz=$zahl;
										}
										if(!strlen($tmpz))
											$tmpz='&nbsp;';
										$tmp .= $tmpz;
									}
									else
									{
										$vl=$GLOBALS['ittabs'].'			'.get_array_entry($GLOBALS['_masknumber'], $feldwert, $GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g], $mskhelpmode, $GLOBALS['bodyzeilenindex'], $GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['status_maske'], '', $tc, $showid);
										$tmp .= $vl.$GLOBALS['nl'];
									}
									// Betragsfeld prüfen
									if(strlen($summenspalten) && (strpos($summenspalten, '+'.substr('00000'.(int)$cnr, -5)) > 0))
									{
										$summe=true;
										if(strlen($feldwert_override))
											$swert=$feldwert_override;
										else
											$swert=$feldwerte[$aktzeile][strtolower($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname'])];
										if(substr($swert, -1) != '^')
										{
											// Einträge aufsummieren
											if($aktzeile == $erstezeile)
												$erstezeilesumme[$cnr]=round((double) $swert, $GLOBALS['fxpglobals']['settings']['nkstellen']);
											else
												$betragssumme[$cnr] += round((double) $swert, $GLOBALS['fxpglobals']['settings']['nkstellen']);
										}
									}
								}
								else
								{
									$vl=$GLOBALS['ittabs'].'			'.get_array_entry($GLOBALS['_masknumber'], $feldwert, $GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g], 2, $GLOBALS['bodyzeilenindex'], $GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['status_maske'], '', $tc, $showid);
									$tmp .= $vl.$GLOBALS['nl'];
								}
							}
							else
							{
								$tmp .= $GLOBALS['ittabs'].'			'.get_array_entry($GLOBALS['_masknumber'], $feldwert, $GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g], $mskhelpmode, $GLOBALS['bodyzeilenindex'], $GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['status_maske'], '', $tc, $showid).$GLOBALS['nl'];
							}
						}

						if(($feldwert == '') && ($maskenart > 3) && ($maskenart < 6) && !$mskhelpmode)
						{
							$tmp .= '&nbsp;';
						}
					}
					$gnr++;
				}
				$cnr++;

				if($GLOBALS['tdopen'] > 0)
				{
					// Hiddenfields einfügen
					if(!$hidden)
					{
						$hidden=true;
						if(fxIsArray($GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]) && isset($GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]['hidden']) && (strlen($GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]['hidden']) > 0))
						{
							$hex=explode(',', $GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]['hidden']);
							$hcnt=0;
							$hsize=sizeof($hex);
							while($hcnt < $hsize)
							{
								$g=(int) $hex[$hcnt];
								if($maskenart == 1) // normale Maske
								{
									if(isset($GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]))
										$feldwert=$feldwerte[$aktzeile][strtolower($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname'])];
									else
										$feldwert=$maskenfeldwerte[strtolower($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname'])];
								}
								else
									$feldwert=$feldwerte[$aktzeile][strtolower($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname'])];
								$feldwert=trim($feldwert);
								$tmp .= $GLOBALS['ittabs'].'			'.get_array_entry($GLOBALS['_masknumber'], $feldwert, $GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g], $mskhelpmode, $GLOBALS['bodyzeilenindex'], $GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['status_maske'], '', $tc, $showid).$GLOBALS['nl'];
								$hcnt++;
							}
						}
					}
					if($tilde && $tdwaehrung)
						$tdwaehrung='&nbsp;';
					if($GLOBALS['tdopen'] > 0)
						$tmpz=$GLOBALS['ittabs'].'		'.close_td($tdwaehrung, 'col'.$showid);
					$tmp .= $tmpz;
				}
			}
			$GLOBALS['tdopen']=-666;
			$tmpz='';
			if(!$GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['listenansicht'] && ($line < $maxzeilenindex))
			{
				$tmpz .= $GLOBALS['ittabs'].'	'.trc(true);
				$tmpz .= $GLOBALS['ittabs'].'</table>'.$GLOBALS['nl'];
				$tmpz .= $GLOBALS['ittabs'].'<!-- DBI: Inner table (end) -->'.$GLOBALS['nl'];
				$GLOBALS['ittabs']=substr($GLOBALS['ittabs'],0,-3);
				$tmpz .= $GLOBALS['ittabs'].'		'.tdc(true);
			}
			if(!$Buttonleiste && ($GLOBALS['tropen'] > 0))
			{
				$tmpz .= $GLOBALS['ittabs'].'	'.trc(true);
			}
			$GLOBALS['tropen']=-666;
			$tmp .= $tmpz;
			$lnr++;
		}

		$msk_bd_str .= $tmp;
//echo('<pre>'.fxHtmlEncode($tmp).'</pre>');
	}

	// Betragszeile ausgeben, falls ermittelt und nicht explizit ausgeblendet
	if($summe && !isset($GLOBALS['msk'.$GLOBALS['_masknumber'].'_nosum']))
	{
		$tmp=tro($aktzeile,true);
		if(!$GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['listenansicht'])
		{
			$tmp .= tdo().'>'.$GLOBALS['nl'];
			$tmp .= '<table id="mt'.$GLOBALS['_masknumber'].'_summe" border="0" cellpadding="0" cellspacing="0">'.$GLOBALS['nl'];
			$tmp .= tro($aktzeile,true);
		}

		$GLOBALS['columncounter']=0;
		$GLOBALS['columnspacer']=true;

		$slc=0;
		foreach($cex as $cnr => $column)
		{
			if(strpos($column, '*') > 0)
				continue;

			if($maskenliste)
				$GLOBALS['columncounter']=$cnr;

			$column=(int)$column;
			$g=(int)$GLOBALS['fx_mfieldsarray'][$GLOBALS['_masknumber']][1][$column];
			if(!$GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['listenansicht'])
			{
				if($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['spaltenanzahl'] > 1)
					$width=(int) ($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['spaltenanzahl']);
				else if($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['spaltenanzahl'] < -20)
					$width=abs($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['spaltenanzahl']).'%';
				else
					$width=0;
			}
			else
				$width=$GLOBALS['mfieldsinfo'][$GLOBALS['_masknumber']]['width'][$column];

			if(strlen($summenspalten) && (strpos($summenspalten, '+'.substr('00000'.(int)$cnr, -5)) > 0))
			{
				$GLOBALS['columnspacer']=true;
				if(($maskenart > 3) && ($maskenart < 6)) // Bei Reports nur Farbe
					$gcc='bcr3';
				else
					$gcc='rs';
				$gci=get_columninfo($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g], $aktzeile, $gcc, $width, 'left', ' NOWRAP', $fld_tooltip);
//echo('<pre>'.fxHtmlEncode($gci).'</pre>');
				$tmp .= strtr($gci, array('border-right'=>'border-top:1px solid #'.$GLOBALS['msk_tbcol'].';border-bottom:1px solid #'.$GLOBALS['msk_sbcol'].';border-right', '#'.$GLOBALS['msk_nbcol']=>'#'.$GLOBALS['msk_sbcol'])).$GLOBALS['nl'];
				if($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldtyp'] == 11)
				{
					$tmp .= '<table border="0" cellpadding="0" cellspacing="0">'.$GLOBALS['nl'];
					$tmp .= tro($aktzeile,true);
					$tmp .= tdo().' nowrap>'.$GLOBALS['nl'];
				}
				if($GLOBALS['fxptdata']['marray'][$mskidname][$GLOBALS['_maskcounter']]['seitenr'] == 0) // Erste Seite des Reports
				{
					$gesamtsumme=$erstezeilesumme[$cnr] + $betragssumme[$cnr];
					$GLOBALS['fxptdata']['marray'][$mskidname][$GLOBALS['_maskcounter']]['betrag_z'][$cnr]=$erstezeilesumme[$cnr];
					$GLOBALS['fxptdata']['marray'][$mskidname][$GLOBALS['_maskcounter']]['betrag_w'][$cnr]=$gesamtsumme;
				}
				else // restliche Seiten des Reports
				{
					if($GLOBALS['Button_Weiter_x'] || $GLOBALS['Button_Weiter_y']) // Weiter-Button
					{
						if($maskenart < 4)
							$gesamtsumme=$GLOBALS['fxptdata']['marray'][$mskidname][$GLOBALS['_maskcounter']]['betrag_w'][$cnr] + $erstezeilesumme[$cnr] + $betragssumme[$cnr];
						else
							$gesamtsumme=$GLOBALS['fxptdata']['marray'][$mskidname][$GLOBALS['_maskcounter']]['betrag_w'][$cnr] + $betragssumme[$cnr];
					}
					else if($GLOBALS['Button_Zurueck_x'] || $GLOBALS['Button_Zurueck_y']) // Zurueck-Button
					{
						if($maskenart < 4)
							$gesamtsumme=$GLOBALS['fxptdata']['marray'][$mskidname][$GLOBALS['_maskcounter']]['betrag_z'][$cnr]  - $erstezeilesumme[$cnr];
						else
							$gesamtsumme=$GLOBALS['fxptdata']['marray'][$mskidname][$GLOBALS['_maskcounter']]['betrag_z'][$cnr];
					}
					else
						$gesamtsumme=$GLOBALS['fxptdata']['marray'][$mskidname][$GLOBALS['_maskcounter']]['betrag_z'][$cnr] + $betragssumme[$cnr];
					$GLOBALS['fxptdata']['marray'][$mskidname][$GLOBALS['_maskcounter']]['betrag_z'][$cnr]=$gesamtsumme - $betragssumme[$cnr];
					$GLOBALS['fxptdata']['marray'][$mskidname][$GLOBALS['_maskcounter']]['betrag_w'][$cnr]=$gesamtsumme;
				}
				$zahl=get_entry((string) $gesamtsumme, $GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]);
				$red=false;
				if(substr($zahl, 0, 1) == '-')
				{
					$red=true;
					$tmp .= '<font class="red">'.$zahl.'</font>';
					$GLOBALS['negnumber']=true;
				}
				else
					$tmp .= $zahl;
				$tmp .= tdc();
				if($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldtyp'] == 11)
				{
					$tmp .= tdo().' nowrap>';
					if($red)
						$tmp .= '<font class="red">';
					$tmp .= '&nbsp;'.$waehrung;
					if($red)
						$tmp .= '</font>';
					$tmp .= tdc(true);
					$tmp .= trc(true);
					$tmp .= '</table>'.$GLOBALS['nl'];
					$tmp .= tdc(true);
					$GLOBALS['negnumber']=false;
				}
				$slc=1;
			}
			else if(($column == 1) || ($g > 0))
			{
				$gci=get_columninfo($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g], $aktzeile, '').'</td>';
//echo('<pre class="grey">$column='.$column.', $g='.$g.':'.$GLOBALS['nl'].fxHtmlEncode($gci).'</pre>');
				$tmp .= strtr($gci, array('border-right'=>'border-top', '#'.$GLOBALS['msk_nbcol']=>'#'.$GLOBALS['msk_tbcol']));
				$slc=0;
			}
		}

		if(!$GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['listenansicht'])
		{
			$tmp .= trc(true);
			$tmp .= '</table>'.$GLOBALS['nl'];
			$tmp .= tdc(true);
		}
		$tmp .= trc(true);
		$msk_bd_str .= $tmp;
	}

	// innere Tabelle wieder schließen?
	if(!$inner_table_closed)
	{
		$msk_bd_str .= $GLOBALS['mttabs'].'</table>'.$GLOBALS['nl'];
		$GLOBALS['mttabs']=substr($GLOBALS['mttabs'],0,-3);
	}

	$msk_bd_str .= $GLOBALS['mttabs'].'					</div>'.$GLOBALS['nl'];
	if($body_closed)
		$msk_bd_str .= $GLOBALS['mttabs'].'					<!-- DBI: Button div (end) -->'.$GLOBALS['nl'];
	else
	{
		$msk_bd_str .= $GLOBALS['mttabs'].'					<!-- DBI: Body div (end) -->'.$GLOBALS['nl'];
		$body_closed=false;
	}

	// Zurueck- bzw. Weiter-Button anzeigen, falls in Listenmodus erforderlich
	// bzw. Download-Button anzeigen, falls Report
	$benbuttons='button_'.$GLOBALS['_masknumber'];
	$bentxtbuttons='txtbutton_'.$GLOBALS['_masknumber'];

	// Benutzerbuttons Text
	if(fxIsArray($GLOBALS[$bentxtbuttons]))
	{
		$msk_bd_str .= $GLOBALS['mttabs'].'							<!-- DBI: Button div (start) -->'.$GLOBALS['nl'];
		$msk_bd_str .= $GLOBALS['mttabs'].'							<div class="maskb" style="position:relative;display:block;">'.$GLOBALS['nl'];
		foreach($GLOBALS[$bentxtbuttons] as $akt_btb)
		{
			$button_value=get_text($akt_btb['fldid'], $akt_btb['fldtxt']);
			$button_width=$btnwidth[$akt_btb['fldtxt']];
			$msk_bd_str .= $GLOBALS['mttabs'].'								'.fieldSubmit($akt_btb['fldname'],$button_value).$GLOBALS['nl'];
			foreach($akt_btb as $key => $value)
			{
				if(($key == 'fldname') || ($key == 'fldid') || ($key == 'fldtxt'))
					continue;
				$msk_bd_str .= fieldHidden($key,$value);
			}
		}
		$msk_bd_str .= $GLOBALS['mttabs'].'							</div>'.$GLOBALS['nl'];
		$msk_bd_str .= $GLOBALS['mttabs'].'							<!-- DBI: Button div (end) -->'.$GLOBALS['nl'];
	}
	else if($reportdownloadbutton || (fxIsArray($GLOBALS[$benbuttons])) || ((($maskenart == 2) || ($maskenart == 4)) && (($erstezeile > 0) || ($letztezeile < $allerletztezeile))))
	{
		$disp_bs='';

		// Button Zurueck
		if((($maskenart == 2) || ($maskenart == 4)) && (($erstezeile > 0) || ($letztezeile < $allerletztezeile)))
		{
			$dis=false;
			if($erstezeile == 0)
				$dis=true;
			$disp_bs .= $GLOBALS['mttabs'].'								'.fieldSubmitImage('Button_Zurueck',$GLOBALS['gfxpath'].'it_back.png','',meldung(679),$dis,$dis,'','',' style="position:absolute;left:4px;"').$GLOBALS['nl'];
		}

		// Download-, Druck- oder Benutzerbuttons
		if($reportdownloadbutton || fxIsArray($GLOBALS[$benbuttons]))
		{
			if(fxIsArray($refw))
				$GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['refw']=$refw;

			// Beim Projektreport sowie Rechnung keinen Datei-Manager anzeigen
			if(($GLOBALS['_masknumber'] == '277') || !$d_manager)
				$d_manager_set=false;
			else
				$d_manager_set=true;

			if(($GLOBALS['_masknumber'] != '277') && $reportdownloadbutton && !$d_manager_set)
			{
				// Download
				$fm_action='loader.php?url=';
				$fm_arg='';
				if($d_manager)
					$fm_action .= 'd_manager.inc';
				else
					$fm_action .= 'report_download.inc';

				// Report downloaden
				$li=fieldLinkImage('icon_download','im_dm',$fm_action.$fm_arg,'<b>'.meldung(1454).'</b>: '.maskHeadlineEntries($headline,false));
				$GLOBALS['fxpvars']['ibuffer'][]=$li;
			}

			// Benutzer-Buttons
			if(fxIsArray($GLOBALS[$benbuttons]))
			{
				$bb_array=$GLOBALS[$benbuttons];
//fxDebug($bb_array,'$bb_array');
				foreach($bb_array as $bb_nr => $benbutton)
				{
					if($benbutton['picname'])
					{
						if($benbutton['name'])
							$bb_name=$benbutton['name'];
						else
							$bb_name='benbutton_'.$GLOBALS['_masknumber'].'_'.$bb_nr;

						if($reportdownloadbutton)	// Report
						{
							$li=fieldSubmitImage($bb_name,$benbutton['picname'],'',$benbutton['tooltip'],false,false,'',$benbutton['href']);
							$GLOBALS['fxpvars']['ibuffer'][]=$li;
						}
						else // Liste
						{
							$li=fieldLinkImage($bb_name,$benbutton['picname'],$benbutton['href'],$benbutton['tooltip'],$benbutton['dtr']);
							$GLOBALS['fxpvars']['ibuffer'][]=$li;
						}
					}
				}
			}

			if($reportdownloadbutton)
			{
				// Druck
				$fm_action='loader.php?url=';
				if($GLOBALS['_masknumber'] == '227')		// Projektstrukturplan (PSP)
					$fm_action .= 'd_manager_psp.inc';
				else if($GLOBALS['_masknumber'] == '533')	// Netzplandiagramm
					$fm_action .= 'd_manager_nzp.inc';
				else if($d_manager)
					$fm_action .= 'd_manager.inc';
				else
					$fm_action .= 'report_download.inc';

				$fm_arg='&tr='.$GLOBALS['fxpglobals']['tr'];

				$rchid=(int)$GLOBALS['Rechnungs_ID_akt'];
				$rchdok='';
				$rchfn=false;
				$idx=-1;
				if(($GLOBALS['_masknumber'] == '277') && $rchid)	// Rechnung
				{
					$fm_arg .= '&Rechnungs_ID='.$rchid;

					$rsql="SELECT mandanten_id, rechnn_f, ausgegangen_am, stichtag, archiv, zeitstempel FROM rechnung WHERE rechnungs_id=".$rchid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$rdat=db_values($rsql);
//fxDebug($rdat, $rsql, 0);
					if(fxIsArray($rdat))
					{
						// Rechnung hat noch kein Ausgangsdatum und Stichtag liegt vor heute -> Rechnung kann nicht mehr erzeugt werden!
						if(!strlen($rdat['ausgegangen_am']) && strlen($rdat['stichtag']) && (substr($rdat['stichtag'],0,8) < substr($GLOBALS['datetime'],0,8)))
						{
							$d_manager=false;
							$reportdownloadbutton=false;
						}

						$dsql  = "SELECT dok_id, dok_originalname, elter_art, elter, dok_art, anlage_dtm, zeitstempel FROM d_dokumente";
						$dsql .= " WHERE elter_art=8 AND dok_art=2580 AND id=".$rchid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						$ddat=db_values($dsql);
//fxDebug($ddat, $dsql);
						if(fxIsArray($ddat))
						{
							$vsql="SELECT dok_verlauf_id, pfad, datei_name, zeitstempel FROM d_dok_verlauf WHERE dok_id=".$ddat['dok_id']." AND aktiv_kz=1 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
							$vdat=db_values($vsql);
//fxDebug($vdat, $vsql);
							if(fxIsArray($vdat))
							{
								if(!isset($GLOBALS['doc']))
									$GLOBALS['doc']=new fxpDoc();
								$rchfn=$GLOBALS['doc']->cpath.$vdat['pfad'];
//fxDebug($rchfn,'$rchfn');
								if(!file_exists($rchfn))
								{
									$fm_arg .= '&rcp=1';
									$rchfn=false;
								}

								// Link
								$dpa=$GLOBALS['doc']->getDocPath('8_2580', $ddat);
//fxDebug($dpa,'$dpa');
								$apt=fxReplaceSpecialCharacters($dpa['full'].$GLOBALS['doc']->fs.$vdat['pfad'],false);
								$aid=$dpa['id'].'.'.$ddat['dok_id'].'.'.$vdat['dok_verlauf_id'];
								$afn=fxReplaceSpecialCharacters($vdat['datei_name'],false);
								$rchdok=' '.fxf_jsFunction('dmsViewer', 'dsw', 'spath='.$apt.'&seid='.$aid.'&sname='.$afn);
//fxDebug($rchdok,'$rchdok');

								// Übergabe-Variablen
								$idx=(int)$ddat['dok_id'];
							}
						}
					}
				}
				$fm_arg .= '&_masknumber='.$GLOBALS['_masknumber'];
				if($repgen)
					$fm_arg .= '&listensql='.urlencode($repsql);
				else
					$fm_arg .= '&listensql='.urlencode($GLOBALS['listensql']);
				if($GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']])
					$fm_arg .= '&msk_counter='.$GLOBALS['_maskcounter'];
				if(strlen($zusatz))
					$fm_arg .= '&zusatz='.$zusatz;
				$fm_arg .= '&headline='.urlencode(maskHeadlineEntries($reportheadline,false));

				if($d_manager)
				{
					// Rechnung $var1 ist nicht mehr im DMS-Ordner vorhanden!<br />Eine Kopie erzeugen?
					if($rchdok && !$rchfn)
					{
						$rch_tt=meldung(271, true, '&quot;<b>'.$rdat['rechnn_f'].'</b>&quot;');
						$rch_btn=get_text(1048,'text').'?';	// Rechnungskopie
						$GLOBALS['fxpvars']['ibuffer'][]=fieldLinkImage('icon_dmanager','im_dm',$fm_action.$fm_arg.'&copy=1','<b>'.$rch_btn.'</b><br /><br />'.$rch_tt);
					}
					else
					{
						// Rechnungsnummer
						if(strlen($rchdok) && ($idx >= 0))
						{
							$li='<img src="'.$GLOBALS['gfxpath'].'im_in.png" tooltip="<b>'.get_text(1323,'20').'</b>: ['.$rdat['rechnn_f'].']"'.$rchdok.' style="cursor:pointer;">';
							$GLOBALS['fxpvars']['ibuffer'][]=$li;
						}
						// Datei-Manager
						$li=fieldLinkImage('icon_dmanager','im_dm',$fm_action.$fm_arg,'<b>'.meldung(1578).'</b>: '.maskHeadlineEntries($headline,false));
						$GLOBALS['fxpvars']['ibuffer'][]=$li;
					}
				}
				else if($reportdownloadbutton)	// Report drucken
				{
					$li=fieldLinkImage('icon_print',$GLOBALS['gfxpath'].'ic_dr.png',$fm_action.$fm_arg,'<b>'.meldung(1455).'</b>: '.maskHeadlineEntries($headline,false));
					$GLOBALS['fxpvars']['ibuffer'][]=$li;
				}
			}
		}

		// Button weiter
		if((($maskenart == 2) || ($maskenart == 4)) && (($erstezeile > 0) || ($letztezeile < $allerletztezeile)))
		{
			$dis=false;
			if($letztezeile >= $allerletztezeile)
				$dis=true;
			$disp_bs .= $GLOBALS['mttabs'].'								'.fieldSubmitImage('Button_Weiter',$GLOBALS['gfxpath'].'it_next.png','',meldung(567),$dis,$dis,'','',' style="position:absolute;right:4px;"').$GLOBALS['nl'];
		}

		if(strlen($disp_bs))
		{
			$msk_bd_str .= $GLOBALS['mttabs'].'							<!-- DBI: Button div (start) -->'.$GLOBALS['nl'];
			$msk_bd_str .= $GLOBALS['mttabs'].'							<div class="maskb" style="position:relative;height:21px;">'.$GLOBALS['nl'];
			$msk_bd_str .= $disp_bs;
			$msk_bd_str .= $GLOBALS['mttabs'].'							</div>'.$GLOBALS['nl'];
			$msk_bd_str .= $GLOBALS['mttabs'].'							<!-- DBI: Button div (end) -->'.$GLOBALS['nl'];
		}
	}

	// Äussere Tabelle wieder schliessen
	$msk_bd_str .= shutdown_mask($GLOBALS['_masknumber']);

	// Checkbox (nur bei Listen laut WB am 08.03.2013)
//fxDebug($GLOBALS['fxptdata']['chkf_'.$GLOBALS['_masknumber']], 'chkf_'.$GLOBALS['_masknumber'].', $maskenart='.$maskenart);
	$maske_nocheckboxes='maske'.$GLOBALS['_masknumber'].'_ncb';
	if((!isset($GLOBALS[$maske_nocheckboxes]) || !$GLOBALS[$maske_nocheckboxes]) && ($maskenart > 1) && sizeof($GLOBALS['fxptdata']['chkf_'.$GLOBALS['_masknumber']]))
	{
		$valid_chk=0;
		foreach($GLOBALS['fxptdata']['chkf_'.$GLOBALS['_masknumber']] as $cfn => $cec)
			$valid_chk += $cec;
		if($valid_chk > 1)
		{
			if($GLOBALS['fxpglobals']['lang'] == 1)
				$cbtt='Checkboxen setzen';
			else
				$cbtt='Mark checkboxes';
			$msk_hd_str=str_replace('<!--checkbox'.$GLOBALS['_masknumber'].'-->', '<img id="icbpu'.$GLOBALS['_masknumber'].'" src="'.$GLOBALS['gfxpath'].'im_checkbox.png" style="cursor:pointer;margin-right:4px;" tooltip="'.$cbtt.'">', $msk_hd_str);
		}
	}

	// Filter
	$GLOBALS['gmskfilter'] |= $GLOBALS['mskfilter'];
	if($GLOBALS['mskfilter'] && ($GLOBALS['_masknumber'] != 454))
	{
		if(!strlen($ci))
		{
			$flt=filter_icon();
			$msk_hd_str=str_replace('<!--filter-->', $flt, $msk_hd_str);
		}

		$GLOBALS['mskfilter']=0;
	}

	// Seitenzähler
	if(!strlen($ci))
	{
		$msk_hd_str=str_replace('<!--anzahl-->', $maxcounts, $msk_hd_str);
		if($maxcounts)
		{
			$pagenr=1+$GLOBALS['fxptdata']['marray'][$mskidname][$GLOBALS['_maskcounter']]['seitenr'];
			$maxpages=ceil($maxcounts/$GLOBALS['fxpglobals']['settings']['listeneintraege']);
			$spage=strpos($msk_hd_str, '<!--spage-->');
			$epage=strpos($msk_hd_str, '<!--epage-->');
			if($spage && $epage)
			{
				$repstr  = '<table border="0" cellspacing="0" cellpadding="0"><tr><td width="100%" align="left">';
				$repstr .= substr($msk_hd_str, $spage+12, $epage-$spage-12);
				$repstr .= '</td><td ALIGN="right"><FONT CLASS="s2b">'.$pagenr.'/'.$maxpages.'</FONT></td></tr></table>';
				$msk_hd_str=substr($msk_hd_str, 0, $spage).$repstr.substr($msk_hd_str, $epage+12);
			}
		}
	}

	// Button-Maske
	$button_maske='maske'.$GLOBALS['_masknumber'].'_bt';
	if(isset($GLOBALS[$button_maske]) && $GLOBALS[$button_maske] && strlen($GLOBALS['fxpvars']['bbuffer'][$GLOBALS['_masknumber']]))
	{
//echo('<HR><B>Button-Maske:</B> '.$GLOBALS['_masknumber'].'<br /><br />');
		$msk_hd_str='<table width="0" height="0" border="0" align="right"><tr><td width="0" height="0">';
		$msk_bd_str='<div style="display:none;width:0;height:0;">'.$GLOBALS['fxpvars']['bbuffer'][$GLOBALS['_masknumber']].'</div>';
		$msk_bd_str .= '</td></tr></table>'.$GLOBALS['nl'];
	}

	// Masken-Status
	if($GLOBALS['__DGBLVL'] == 8)
	{
		$mst=(int)$GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['status_maske'];
		$mstatus=array('*undefiniert*', 'Entwurf', 'PGM nicht OK-Test', '<FONT COLOR="#BB0011">PGM OK-Test</FONT>', '<FONT COLOR="#1111BB">PGM OK-Integration</FONT>', '<FONT COLOR="#00BB11">PGM OK-Produktion</FONT>');
		echo('<B>Maske Nr. '.$GLOBALS['_masknumber'].':</B> Status='.$mst.' ('.$mstatus[$mst].')<br />');
	}

	if(isset($GLOBALS['fxpvars']['lookups'][$GLOBALS['_masknumber']]))
		unset($GLOBALS['fxpvars']['lookups'][$GLOBALS['_masknumber']]);
	$GLOBALS['_maskcounter']++;

	// Header speichern
	if(fxIsArray($GLOBALS['fx_mheaders'][$mnr]))
		saveSessionHeaders($mnr,$GLOBALS['fx_mheaders'][$mnr]);

	return $msk_hd_str.$msk_bd_str;
}

/**
 * shutdown_mask
 * Diese Funktion schließt die Maske.
 * -----------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @param string $mnr - Optional parameter (default = ''): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function shutdown_mask($mnr='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($mnr))
		$mnr=$GLOBALS['_masknumber'];
	else
		$mnr=(string)(trim($mnr));

	$html ='										</div>'.$GLOBALS['nl'];
	$html .= '										<!-- DBI: Main div (end) -->'.$GLOBALS['nl'];
	if(isset($_POST['mt_hl']) && $_POST['mt_hl'])
	{
			$html .= '								<div style="position:absolute;left:14px;top:-8px;width:0;height:108%;border-left:1px solid #888;"></div>'.$GLOBALS['nl'];
			$html .= '								<div style="position:absolute;left:'.($GLOBALS['fxpglobals']['settings']['standardbreite']+14).'px;top:-8px;width:0;height:108%;border-left:1px solid #888;"></div>'.$GLOBALS['nl'];
	}
	$html .= '									</td>'.$GLOBALS['nl'];
	$html .= '								</tr>'.$GLOBALS['nl'];
	$html .= '							</table>'.$GLOBALS['nl'];
	$html .= '							<!-- DBI: Main table (end) -->'.$GLOBALS['nl'];

	// Clear, wenn $maske???_cl angegeben
	$msk_clear='maske'.$mnr.'_cl';
	if(isset($GLOBALS[$msk_clear]) && (gettype($GLOBALS[$msk_clear]) == 'string'))
		$html .= '							<br clear="'.$GLOBALS[$msk_clear].'">'.$GLOBALS['nl'];

	return $html;
}

/**
 * get_array_entry
 * Diese Funktion stellt die Inhalte der Feldwerte aus der Datenbank ent-
 * sprechend der gewünschten Anzeigeart dar, welche wiederum aus der DB geholt
 * wird.
 * -> wird für die Darstellung der Maske benötigt
 * -----------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @param various $mnr          - Mandatory parameter:                   ???PARAMETER???
 * @param various $feldwert     - Mandatory parameter:                   ???PARAMETER???
 * @param various $array        - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $mskhilfe     - Optional parameter (default = false):  ???PARAMETER???
 * @param string  $bzindex      - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $maskenstatus - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $report       - Optional parameter (default = ''):     ???PARAMETER???
 * @param integer $tc           - Optional parameter (default = 0):      ???PARAMETER???
 * @param string  $showid       - Optional parameter (default = ''):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_array_entry($mnr, $feldwert, $array, $mskhilfe=false, $bzindex='', $maskenstatus='', $report='', $tc=0, $showid='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Wenn kein Array übergeben wurde, dann Zurueck...
	if(!fxIsArray($array))
		return $feldwert;

	$maskenart=$GLOBALS['fx_mask'][$mnr]['maskenart'];

	$anzart=$GLOBALS['anzart_defines'][$array['anzeigeart']];

	$bzkey='';
	$bzlnr='';
	$bzexa=explode('[', substr($bzindex, 1));
	if(sizeof($bzexa) == 2)
	{
		$bzkey=(int)$bzexa[0];
		$bzlnr=(int)$bzexa[1];
	}

	// Prüfen, ob Feldwert überhaupt angezeigt werden soll
	$not_visible=$array['feldname'].'_nv';
	if(isset($GLOBALS[$not_visible]))
		$nva=$GLOBALS[$not_visible];
	else
		$nva=false;
	if(fxIsArray($nva) && strlen($bzkey))
		$nva=$nva[$bzkey][$bzlnr];
	if($nva == 1)
	{
		return '';
	}
//fxDebug($nva, $array['feldname'].'_nv');

	// Prüfen, ob Feld disabled werden soll
	$dis=false;
	$disable_it=$array['feldname'].'_dis';
	if(isset($GLOBALS[$disable_it]) && $GLOBALS[$disable_it])
		$dis=true;
	if(!$dis && strlen($bzkey))
	{
		$disable_it=$array['feldname'].$bzlnr.'_dis';
		if(isset($GLOBALS[$disable_it]) && $GLOBALS[$disable_it])
			$dis=true;
		else
		{
			$disable_it='row'.$bzlnr.'_dis';
			if(isset($GLOBALS[$disable_it]) && $GLOBALS[$disable_it])
				$dis=true;
		}
	}
//echo('<b>'.$array['feldname'].'</b>'.$bzlnr.': disabled=['.$dis.']<br />');

	// Prüfen, ob Tooltip für dieses Feld hinterlegt wurde
	$tooltip='';
	$tt=$array['feldname'].'_tt';
	if(isset($GLOBALS[$tt]) && strlen($GLOBALS[$tt]))
		$tooltip=$GLOBALS[$tt];
	if(!strlen($tooltip) && strlen($bzkey))
	{
		$tt=$array['feldname'].$bzlnr.'_tt';
		if(isset($GLOBALS[$tt]) && strlen($GLOBALS[$tt]))
			$tooltip=$GLOBALS[$tt];
	}
//echo('<b>'.$array['feldname'].'</b>'.$bzindex.': tooltip=['.fxHtmlEncode($tooltip).']<br />');

	// Prüfen, ob JavaScript Funktion hinzugefügt werden soll
	$jsfunc='';
	$jsfunc_it=$array['feldname'].'_jf';
	if(isset($GLOBALS[$jsfunc_it]) && strlen($GLOBALS[$jsfunc_it]))
		$jsfunc=$GLOBALS[$jsfunc_it];
	if(!strlen($jsfunc) && strlen($bzkey))
	{
		$jsfunc_it=$array['feldname'].$bzlnr.'_jf';
		if(isset($GLOBALS[$jsfunc_it]) && strlen($GLOBALS[$jsfunc_it]))
			$jsfunc=$GLOBALS[$jsfunc_it];
	}
//echo('<b>'.$array['feldname'].'</b>'.$bzindex.': jsfunc=['.fxHtmlEncode($jsfunc).']<br />');

	// Literal zuweisen für Report-Funktion
	if($report !== '')
		$anzart=$GLOBALS['anzart_defines'][$report];

	// Prüfen auf Override-Variable für Muss-/Kannfelder
	$mkfield=$array['feldname'].'_muss';
	if(isset($GLOBALS[$mkfield]) && (gettype($GLOBALS[$mkfield]) == 'boolean'))
	{
		if($GLOBALS[$mkfield] == true)
			$mussfeld=true;
		else
			$mussfeld=false;
	}
	else if($array['muss'])
		$mussfeld=true;
	else
		$mussfeld=false;

	// Änderungsfelder im Anzeige- oder Löschen-Modus werden zu reinen Anzeigefeldern
	$changeinput=false;

	$inputfield='maske'.$mnr.'_sf';
	$changemaske=false;
	if(isset($GLOBALS[$inputfield]) && ($GLOBALS[$inputfield] === true))
		$changemaske=true;
	else if(isset($GLOBALS[$inputfield]) && ($GLOBALS[$inputfield] === 1))	// Alle Felder auf "DISABLED" setzen
		$nva=true;
	else if(isset($GLOBALS[$inputfield]) && ($GLOBALS[$inputfield] == 2))	// Gesamte Maske im Anzeige-Modus
	{
		$changemaske=true;
		$inputfield=$array['feldname'].'_sf';
		if(!isset($GLOBALS[$inputfield]))
			$inputfield=$array['feldname'].$bzlnr.'_sf';
		if(!isset($GLOBALS[$inputfield]) || (gettype($GLOBALS[$inputfield]) != 'integer'))
		{
			if(($anzart == 'textfeld, einzeilig') || ($anzart == 'textfeld, mehrzeilig'))
				$anzart='nur lesen-textfeld';
		}
		$changeinput=true;
	}

	if(!$changemaske)
	{
		$inputfield=$array['feldname'].'_sf';
		if(!isset($GLOBALS[$inputfield]))
			$inputfield=$array['feldname'].$bzlnr.'_sf';
		$changefield=0;
		if((!isset($GLOBALS[$inputfield]) || ($GLOBALS[$inputfield] === false)) && (($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE)))
			$changefield=1;
		else if(isset($GLOBALS[$inputfield]) && (gettype($GLOBALS[$inputfield]) == 'integer'))
		{
			if($GLOBALS[$inputfield] == 0)
				$nva=true;
			else
				$changefield=$GLOBALS[$inputfield];
		}

		if($changefield)
		{
			if(($anzart == 'textfeld, einzeilig') || ($anzart == 'textfeld, mehrzeilig'))
				$anzart='nur lesen-textfeld';
			$changeinput=true;
		}
	}

	// Override-Variable für die Breite in Styles
	$wfield=$array['feldname'].'_width';
	$fwidth=0;
	if(isset($GLOBALS[$wfield]) && (gettype($GLOBALS[$wfield]) == 'integer'))
		$fwidth=$GLOBALS[$wfield];

	$altname=' TITLE';
	$style='';
//echo('<b>'.$array['feldname'].'</b> = '.$feldwert.' ($anzart='.$anzart.')<hr />');

	// Anzeigeart auswerten
	switch($anzart)
	{
		// Hier werden die unterschiedlichen Literale ausgegeben
		case 'literal_5':
		case 'literal_10':
		case 'literal_15':
		case 'literal_20':
		case 'literal_text':
			$l=getLiteral($array, $feldwert, $report, $showid);
//fxDebug($l,'$l: '.$array['feldname'].' ('.$anzart.')');
			extract($l);

			// Literal merken
			if(($maskenart > 1) && ($maskenart < 6))
			{
				if($array['zeile'] == 1)
				{
					$btmp=strtr($tmp, array('<br />'=>"(*BR*)", '<br />'=>"(*BR*)", '<br />'=>"(*BR*)"));
					$dip=stripos($btmp,'</div>');
					if($dip)
						$stl=strtr(strip_tags(substr($btmp,0,$dip)), array('(*BR*)'=>"<br />"));
					else
						$stl=strtr(strip_tags($btmp), array('(*BR*)'=>"<br />"));
					maskAddCBColumn($GLOBALS['_masknumber'],$fn,$array['spalte'],$stl);
				}
			}
			else if(!isset($GLOBALS['fxptdata']['litf_'.$GLOBALS['_masknumber']][$fn]))
				$GLOBALS['fxptdata']['litf_'.$GLOBALS['_masknumber']][$fn]=strip_tags($tmp);
//echo('$lit:<pre>'.$lit.'</pre>$tmp:<pre>'.fxHtmlEncode($tmp).'</pre><hr />');
		break;

		case 'literal_rahmen':
			$tmp='';
			if(!$array['anzeigegroesse1'])
				$tmp .= '<hr class="fxhr">';
			if(substr($array['feldname'],0,2) != 'hr')
			{
				$l=getLiteral($array, $feldwert, 'literal_20');
//fxDebug($l,'$l: '.$array['feldname'].' ('.$anzart.')');
				$tmp .= '<center><div class="grey" style="font-weight:600;margin:4px 0 6px 0;">'.$l['lit'].'</div></center>';
			}
//echo('$lit:<pre>'.$lit.'</pre>$tmp:<pre>'.fxHtmlEncode($tmp).'</pre><hr />');
		break;

		case 'literal_header':
			$l=getLiteral($array, $feldwert, 'anmerkung');
//fxDebug($l,'$l: '.$array['feldname'].' ('.$anzart.')');
			$tmp='<font class="grey">'.trim($l['lit']).'</font><hr class="fxhr">';
//echo('$lit:<pre>'.$lit.'</pre>$tmp:<pre>'.fxHtmlEncode($tmp).'</pre><hr />');
		break;

		case 'textfeld, einzeilig':
		case 'datei':
			// Breite
			$width=0;
			if($fwidth)
				$width=$fwidth;
			else if($array['anzeigegroesse1'] < 0)
				$width=abs((int)$array['anzeigegroesse1']);
			else if($array['spaltenanzahl'] > 0)
				$width=(int)$array['spaltenanzahl'];
			// File
			if($anzart == 'datei')
			{
				$ftype='';
				if(isset($GLOBALS[$array['feldname'].'_ftype'.$bzindex]))
					$ftype=substr(strtolower($GLOBALS[$array['feldname'].'_ftype'.$bzindex]),0,3);
				else if(isset($GLOBALS[$array['feldname'].'_ftype']))
					$ftype=substr(strtolower($GLOBALS[$array['feldname'].'_ftype']),0,3);
				else if($array['feldname'] == 'bild')
					$ftype='pic';
				else if($array['feldname'] == 'dokument')
					$ftype='doc';
//fxDebug($ftype,'$ftype', 0);
				if($ftype == 'pic')
				{
//fxDebug($array,'$array', 0);
					$tmp=fieldFilePic($array['feldname'].$bzindex, $feldwert, $array['vertausr'], $array['horausr'], $array['spaltenanzahl'], $array['zeilenanzahl'], $array['anzeigegroesse1'], $array['anzeigegroesse2'], $tooltip, $jsfunc);
				}
				else if($ftype == 'doc')
				{
//fxDebug($array,'$array', 0);
					$tmp=fieldFileDoc($array['feldname'].$bzindex, $feldwert, $array['spaltenanzahl'], $array['zeilenanzahl'], $array['anzeigegroesse1'], $tooltip, $jsfunc);
				}
				else
				{
					if(!$width && ((int)$array['anzeigegroesse1'] > 0))
						$width=floor((int)$array['anzeigegroesse1']*6.3);
					$multi=false;
					if($array['zeilenanzahl'] > 1)
						$multi=true;
					$width=(int)$array['spaltenanzahl'];
					$tmp=fieldFile($array['feldname'].$bzindex, $width, $multi, $mussfeld, $dis, $nva, $tooltip, $jsfunc);
				}
			}
			// Date
			else if($array['feldtyp'] == 7)
				$tmp=fieldDate($array['feldname'].$bzindex, get_entry($feldwert, $array), $width, $mussfeld, $dis, $nva, $tooltip, $jsfunc);
			// Password
			else if($array['anzeigegroesse2'] == '1')
				$tmp=fieldPassword($array['feldname'].$bzindex, get_entry($feldwert, $array), $width, $array['laenge'], $mussfeld, $dis, $nva, $tooltip, $jsfunc);
			// Text
			else
			{
				if(isset($GLOBALS[$array['feldname'].'_align']))
					$align=trim($GLOBALS[$array['feldname'].'_align']);
				else
					$align=trim($array['horausr']);
				if(!strlen($align))
					$align='left';
				$tmp=fieldText($array['feldname'].$bzindex, get_entry($feldwert, $array), $width, $array['laenge'], $align, $mussfeld, $dis, $nva, $tooltip, $jsfunc);
			}
		break;

		case 'textfeld, mehrzeilig':
			// Breite
			$width=0;
			if($fwidth)
				$width=$fwidth;
			else if($array['anzeigegroesse1'] < 0)
				$width=abs((int)$array['anzeigegroesse1']);
			else if($array['spaltenanzahl'] > 0)
				$width=(int)$array['spaltenanzahl'];
			// Höhe
			$height=0;
			if($array['anzeigegroesse2'])
			{
				$ag2=(int)$array['anzeigegroesse2'];
				if(($ag2 > -999) && ($ag2 < 0))
					$height=abs((int)$ag2);
				else
					$height=abs((int)$ag2)*14;
			}
			$tmp=fieldTextarea($array['feldname'].$bzindex, get_entry($feldwert, $array), $width, $height, $array['laenge'], $mussfeld, $dis, $nva, $tooltip, $jsfunc);
		break;

		case 'auswahlfeld':
			// Breite
			$width=0;
			if($fwidth)
				$width=$fwidth;
			else if($array['anzeigegroesse2'] != 0)
				$width=(int)$array['anzeigegroesse2'];
			else if($array['spaltenanzahl'] > 0)
				$width=(int)$array['spaltenanzahl'];
			$tmp=fieldSelect($array['feldname'].$bzindex, $feldwert, $width, $array['anzeigegroesse1'], $mussfeld, $dis, $nva, $tooltip, $jsfunc, $changeinput);
//echo(fxHtmlEncode($tmp));
		break;

		case 'bild':
			// Falls der Bildername in 'html_grafik' mit einem $ beginnt, dann die entspr. Variable verwenden
			$bildname=$array['html_grafik'];
			if(substr($bildname, 0, 1) == '$')
				$bildname=$GLOBALS[substr($array['html_grafik'], 1)];
			else if(substr($bildname, 0, 1) == '§')
				$bildname=$feldwert;

			// Falls der alternative Bildername in 'literal_10' mit einem $ beginnt, dann die entspr. Variable verwenden
			$bildaltname=$array['literal_10'];
			if(substr($bildaltname, 0, 1) == '$')
				$bildaltname=$GLOBALS[substr($array['literal_10'], 1)];
			// Zusätzliche Parameter ermitteln, wie Breite und Höhe des Bildes
			$zusatzparams='';
			if($array['anzeigegroesse1'])
				$zusatzparams .= ' width="'.$array['anzeigegroesse1'].'"';
			if($array['anzeigegroesse2'])
				$zusatzparams .= ' height="'.$array['anzeigegroesse2'].'"';
			// Imagewechsel ermöglichen mit OnMouseout/-over
			$bildover='';

			if(fxIsArray($bildname))
			{
				$bn=$bildname[0];
				$bpp=(int)strpos($bn, '.');
				if(!$bpp)
				{
					$bn .= '.png';
					$bpp=(int)strpos($bn, '.');
				}
				$bildover=' onmouseout=src="'.$GLOBALS['gfxpath'].strtolower(substr($bn, 0, $bpp+4)).substr($bn, $bpp+4).'" onmouseover=src="'.$GLOBALS['gfxpath'].strtolower($bildname[1]).'"';
				$bildname=$bn;
			}

			$bpp=(int)strpos($bildname, '.');
			if(!$bpp)
			{
				$bildname .= '.png';
				$bpp=(int)strpos($bildname, '.');
			}
			$tmp='<img src="'.$GLOBALS['gfxpath'].strtolower(substr($bildname, 0, $bpp+4)).substr($bildname, $bpp+4).'" border="0"'.$bildover.$zusatzparams.$altname.'="'.$bildaltname.'"'.$jsfunc.'>';
		break;

		case 'icon':
			// Falls der Bildername in 'html_grafik' mit einem $ beginnt, dann die entspr. Variable verwenden
			$bildname=strtolower($array['html_grafik']);
			if(substr($bildname, 0, 1) == '$')
				$bildname=$GLOBALS[substr($bildname, 1)];
			else if(substr($bildname, 0, 1) == '§')
				$bildname=strtolower($feldwert);
			// Falls der alternative Bildername in 'literal_10' mit einem $ beginnt, dann die entspr. Variable verwenden
			$bildaltname=$array['literal_10'];
			if(substr($bildaltname, 0, 1) == '$')
				$bildaltname=$GLOBALS[substr($array['literal_10'], 1)];
			// Zusätzliche Parameter ermitteln, wie Breite und Höhe des Bildes
			$zusatzparams='';
			if($array['anzeigegroesse1'])
				$zusatzparams .= ' width="'.$array['anzeigegroesse1'].'"';
			if($array['anzeigegroesse2'])
				$zusatzparams .= ' height="'.$array['anzeigegroesse2'].'"';
			// Imagewechsel ermöglichen mit onMouseOut/-Over/-Down
			$bildover='';
			if(fxIsArray($bildname))
			{
				$bildover=' onmouseout=src="'.$GLOBALS['gfxpath'].strtolower($bildname[0]).'" onmouseover=src="'.$GLOBALS['gfxpath'].strtolower($bildname[1]).'"';
				if($bildname[2])
					$bildover .= ' onmousedown=src="'.$GLOBALS['gfxpath'].strtolower($bildname[2]).'""';
				$bildname=$bildname[0];
			}

			// Falls der Linkname in 'feldname' mit einem $ beginnt, dann die entspr. Variable verwenden
			$bildhrefname=$array['feldname'];
			if(substr($bildhrefname, 0, 1) == '$')
				$bildhrefname=$GLOBALS[substr($bildhrefname, 1)];

			$bpp=(int)strpos($bildname, '.');
			if(!$bpp)
				$bildname .= '.png';
			$tmp='<img src="'.$GLOBALS['gfxpath'].strtolower($bildname).'" border="0"'.$bildover.$zusatzparams.$altname.'="'.$bildaltname.'"'.$jsfunc.'>';
		break;

		case 'nur lesen-textfeld':
		case 'versteckt':
		case 'summe':
			$neg=false;
			if(!is_array($feldwert) && substr($feldwert,0,1) == '-')
				$neg=true;

			if(!is_array($feldwert) && ($feldwert !== '') && ($array['nachschlage'] == 0))
				$f_tmp=nl2br(get_entry($feldwert, $array));
			else
				$f_tmp=$feldwert;
			if(($anzart == 'nur lesen-textfeld') || ($anzart == 'summe'))
			{
				$width=0;
				$tmp = fieldRead($array['feldname'].$bzindex,$f_tmp,$neg,$width);
			}
			else
				$tmp = fieldHidden($array['feldname'].$bzindex,$f_tmp);
		break;

		case 'checkbox':
			if($changeinput)
				$dis=true;
			$tmp=fieldCheckbox($array['feldname'].$bzindex, '', get_entry($feldwert, $array), $mussfeld, $dis, $nva, $tooltip, $jsfunc, '', true);
		break;

		case 'submit-button':
			$force=0;
			if($GLOBALS['check_baseline'])
				$force=false;
			$tmp=fieldSubmit($array['feldname'].$bzindex, $array, $anzart, 0, $dis, $nva, $force, '', '', $jsfunc);
		break;

		case 'reset-button':
//			$tmp=fieldSubmit($array['feldname'].$bzindex, $array, $anzart, 0, $dis, $nva, false, '', '', $jsfunc);
			$tmp='';
		break;

		case 'submit-bild':
			$tmp .= fieldSubmitImage($array['feldname'].$bzindex, $array, $feldwert, '', $dis, $nv, '', '', $jsfunc);
		break;

		default:
			if($maskenstatus == '5')
				return fxHtmlEncode(get_entry($feldwert));
			return '<font color="#f00000">'.$feldwert.'<br />*** ('.$anzart.') ***</font>';
	}

	return $tmp;
}

/**
 * open_table
 * Diese Funktion öffnet eine Tabelle mit den entspr. Eigenschaften: Breite,
 * Border, etc.
 * -> wird für die Darstellung der Maske benötigt
 * -----------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @param various $mnr - Mandatory parameter: ???PARAMETER???
 * @param various $arr - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function open_table($mnr, $arr)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$width='';
	if(fxIsArray($arr) && ($arr['border'] || $arr['listenansicht']))
		$width=' width="100%"';

	$rs='<table id="mt'.$mnr.'_detail"'.$width.' border="0" cellpadding="0" cellspacing="0"';

	if(isset($_POST['mt_hl']) && $_POST['mt_hl'])
		$rs .= ' style="box-shadow:0 0 2px #000000 inset;"';

	$rs .= '>';

	return $rs;
}

/**
 * getColumnspace
 * Autor: Walter_T
 *
 * @param various $mfa         - Mandatory parameter:                   ???PARAMETER???
 * @param various $aktzeile    - Mandatory parameter:                   ???PARAMETER???
 * @param various $headline    - Mandatory parameter:                   ???PARAMETER???
 * @param various $with_style  - Mandatory parameter:                   ???PARAMETER???
 * @param various $width       - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $with_border - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getColumnspace($mfa,$aktzeile,$headline,$with_style,$width,$with_border=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$fld='';
	$ffs=-1;
	$mn=$GLOBALS['_masknumber'];
	$mc=$GLOBALS['_maskcounter'];

	if(fxIsArray($mfa) && isset($mfa['feldname']))
	{
		$fld=$mfa['feldname'];

		$az='headline';
		if(strlen($aktzeile))
			$az=$aktzeile;

		$ff=-1;
		$fl=-1;
		$fc='';

		$el='empty';
		$ln='line';
		$fn=$fld;

		$set=false;

		// Überschrift
		if($az === 'headline')
		{
			if(!$set)
			{
				$fn_mn_az=$fn.$mn.'_'.$az;		// [Feldname][Maskennummer]_headline			z.B. Dateiname351_headline
				$fn_az=$fn.'_'.$az;				// [Feldname]_headline							z.B. Dateiname_headline
				$fn_mc_az=$fn.'__'.$mc.'_'.$az;	// [Feldname]__[Maskenzähler]_headline			z.B. Dateiname__1_headline

				$fa=array($fn_mn_az, $fn_mc_az, $fn_az);
				foreach($fa as $fc)
				{
					if(isset($GLOBALS[$fc.'_ffs']))
					{
						$ff=(int)$GLOBALS[$fc.'_ffs'];
						$ffs=$ff;
						$set=true;
						break;
					}
				}
//if($set) echo('<b>FIELD [HEADLINE]</b> '.$fn.'['.$az.'] (&quot;'.$fc.'&quot; - $mn='.$mn.', $mc='.$mc.') &rarr; $ffs='.$ffs.'<hr size=1 />');
			}
			if(!$set)
			{
				$ln_mn_az=$ln.$mn.'_'.$az;		// line[Maskennummer]_headline					z.B. line351_headline
				$ln_az=$ln.'_'.$az;				// line_headline								z.B. line_headline
				$ln_mc_az=$ln.'__'.$mc.'_'.$az;	// line__[Maskenzähler]_headline				z.B. line__1_headline

				$fa=array($ln_mn_az, $ln_mc_az, $ln_az);
				foreach($fa as $fc)
				{
					if(isset($GLOBALS[$fc.'_ffs']))
					{
						$fl=(int)$GLOBALS[$fc.'_ffs'];
						$ffs=$fl;
						$set=true;
						break;
					}
				}
//if($set) echo('<b>LINE [HEADLINE]</b> '.$fn.'['.$az.'] (&quot;'.$fc.'&quot; - $mn='.$mn.', $mc='.$mc.') &rarr; $ffs='.$ffs.'<hr size=1 />');
			}
		}
		// Liste
		else
		{
			if(!$set)
			{
				$el_mn_az=$el.$mn.'_'.$az;		// empty[Maskennummer]_[Zeilennummer]			z.B. empty351_4
				$el_mc_az=$el.'__'.$mc.'_'.$az;	// empty__[Maskenzähler]_[Zeilennummer]			z.B. empty__1_4

				$fa=array($el_mn_az, $el_mc_az);
				foreach($fa as $fc)
				{
					if(isset($GLOBALS[$fc]) || isset($GLOBALS[$fc.'_ffs']))
					{
						$ffs=0;
						$set=true;
						break;
					}
				}
//if($set) echo('<b>EMPTY</b> '.$fn.'['.$az.'] (&quot;'.$fc.'&quot; - $mn='.$mn.', $mc='.$mc.') &rarr; $ffs='.$ffs.'<hr size=1 />');
			}
			if(!$set)
			{
				$fn_mn=$fn.$mn;				// [Feldname][Maskennummer]							z.B. Dateiname351
				$fn_mn_az=$fn_mn.'_'.$az;	// [Feldname][Maskennummer]_[Zeilennummer]			z.B. Dateiname351_4
				$fn_az=$fn.'_'.$az;			// [Feldname]_[Zeilennummer]						z.B. Dateiname_4
				$fn_mc=$fn.'__'.$mc;		// [Feldname]__[Maskenzähler]						z.B. Dateiname__1
				$fn_mc_az=$fn_mc.'_'.$az;	// [Feldname]__[Maskenzähler]_[Zeilennummer]		z.B. Dateiname__1_4

				$fa=array($fn_mn_az, $fn_mc_az, $fn_az,  $fn_mn, $fn_mc,  $fn);
				foreach($fa as $fc)
				{
					if(isset($GLOBALS[$fc.'_ffs']))
					{
						$ff=(int)$GLOBALS[$fc.'_ffs'];
						$ffs=$ff;
						$set=true;
						break;
					}
				}
//if($set) echo('<b>FIELD</b> '.$fn.'['.$az.'] (&quot;'.$fc.'&quot; - $mn='.$mn.', $mc='.$mc.') &rarr; $ffs='.$ffs.'<hr size=1 />');
			}
			if(!$set)
			{
				$ln_mn=$ln.$mn;				// line[Maskennummer]								z.B. line351
				$ln_mn_az=$ln_mn.'_'.$az;	// line[Maskennummer]_[Zeilennummer]				z.B. line351_4
				$ln_az=$ln.'_'.$az;			// line_[Zeilennummer]								z.B. line_4
				$ln_mc=$ln.'__'.$mc;		// line__[Maskenzähler]								z.B. line__1
				$ln_mc_az=$ln_mc.'_'.$az;	// line__[Maskenzähler]_[Zeilennummer]				z.B. line__1_4

				$fa=array($ln_mn_az, $ln_mc_az, $ln_az,  $ln_mn, $ln_mc);
				foreach($fa as $fc)
				{
					if(isset($GLOBALS[$fc.'_ffs']))
					{
						$fl=(int)$GLOBALS[$fc.'_ffs'];
						$ffs=$fl;
						$set=true;
						break;
					}
				}
//if($set) echo('<b>LINE</b> '.$fn.'['.$az.'] (&quot;'.$fc.'&quot; - $mn='.$mn.', $mc='.$mc.') &rarr; $ffs='.$ffs.'<hr size=1 />');
			}
		}
//if($set) echo('<b>RESULT</b> '.$fn.'['.$az.'] (&quot;'.$fc.'&quot; - $mn='.$mn.', $mc='.$mc.') &rarr; $ffs='.$ffs.'<hr size=1 />');
//echo('<b>RESULT</b> '.$fn.'['.$az.'] (&quot;'.$fc.'&quot; - $mn='.$mn.', $mc='.$mc.') &rarr; $ffs='.$ffs.'<hr size=1 />');
	}

	$rs='';
	if($GLOBALS['fx_mask'][$mn]['listenansicht'])
	{
		if($headline && isset($GLOBALS['maske'.$mn.'_hpd']))
			$rs .= 'padding:'.$GLOBALS['maske'.$mn.'_hpd'].';';
		else if(!$headline && isset($GLOBALS[$fn.'_dpd']))
			$rs .= 'padding:'.$GLOBALS[$fn.'_dpd'].';';
		else if(!$headline && isset($GLOBALS['maske'.$mn.'_dpd']))
			$rs .= 'padding:'.$GLOBALS['maske'.$mn.'_dpd'].';';
		else if(!isset($GLOBALS['maske'.$mn.'_npd']))
			$rs .= 'padding:1px 4px;';
		if($headline)
		{
			$fln=0;
			$fcl=0;
			if(is_array($mfa) && sizeof($mfa) && isset($mfa['zeile']))
			{
				$fln=$mfa['zeile'];
				$fcl=$mfa['spalte'];
			}
			if(isset($GLOBALS['design_mbg']) && strlen($GLOBALS['design_mbg']))	// Masken background color
				$c=$GLOBALS['design_mbg'];
			else if(fxIsArray($GLOBALS['bstyle']) && isset($GLOBALS['bstyle']['maskc']))
				$c=$GLOBALS['bstyle']['maskc'];
			else if(!strlen($GLOBALS['fxpglobals']['design']))
				$c='f0f0f0';
			else
				$c='fafafa';
			if(strlen($c) == 6)
				$c='#'.$c;
			$rs .= 'border-bottom:2px solid '.$c.';';
			if(is_string($fld) && strlen($fld))
			{
				if(isset($GLOBALS['design_hbg']) && strlen($GLOBALS['design_hbg']))	// Headline background color
				{
					$c=$GLOBALS['design_hbg'];
					if(strlen($c) == 6)
						$c='#'.$c;
					$rs .= 'background:'.$c.';';
				}
			}
			else
				$rs .= 'background:'.$c.';';
		}
	}
	else
		$rs .= 'border:0;border-top:2px solid transparent;border-bottom:2px solid transparent;';

	// Frame borders
	if(!isset($GLOBALS['maske'.$mn.'_nbd']))
	{
		// Border...
		$bd=trim($GLOBALS['fx_mask'][$mn]['ausrichtung']);
		// ...none, left or right
		if(strlen($bd))
		{
			if(strtolower(substr($bd,0,1)) == 'l')
				$bd='left';
			else
				$bd='right';
		}

		// Border color
		$b=$GLOBALS['msk_nbcol'];
		if($headline && isset($GLOBALS['design_hbc']) && strlen($GLOBALS['design_hbc']))	// Headline border color
			$b=$GLOBALS['design_hbc'];
		if(!$headline && isset($GLOBALS['design_lbc']) && strlen($GLOBALS['design_lbc']))	// Line border color
			$b=$GLOBALS['design_lbc'];
		if(isset($GLOBALS['msk_nbcol_'.$mn]))
			$b=$GLOBALS['msk_nbcol_'.$mn];
		if(strlen($aktzeile) && isset($GLOBALS['msk_nbcol_'.$mn.'_'.$aktzeile]))
			$b=$GLOBALS['msk_nbcol_'.$mn.'_'.$aktzeile];

		if(strlen($b) == 6)
			$b='#'.$b;
//echo('<b>'.$mn.'</b> ['.$fld.']: $with_border='.$with_border.' - columncounter='.$GLOBALS['columncounter'].', columncountermax='.$GLOBALS['columncountermax'].' - $bd='.$bd.', $ffs='.$ffs.', $b='.$b.'<hr />');

		if($ffs >= 0)
		{
//echo($fld.': $ffs='.$ffs.' - All=15='.($ffs&15).' - Bit 0=1=top='.($ffs&1).', Bit 1=2=right='.($ffs&2).', Bit 2=4=bottom='.($ffs&4).', Bit 3=8=left='.($ffs&8).'<br />');
//$b='#e1001a';
			if(($ffs&15) == 15)
				$rs .= 'border:1px solid '.$b.';';
			else
			{
				if(($ffs&1) == 1)
					$rs .= 'border-top:1px solid '.$b.';';
				if(($ffs&2) == 2)
					$rs .= 'border-right:1px solid '.$b.';';
				if((($ffs&4) == 4) && !$headline)
					$rs .= 'border-bottom:1px solid '.$b.';';
				if(($ffs&8) == 8)
					$rs .= 'border-left:1px solid '.$b.';';
			}
		}
		else if($with_border)
		{
//$b='#009f6b';
			if($bd === 'left')
			{
				if($GLOBALS['columncounter'] > 0)
					$rs .= 'border-'.$bd.':1px solid '.$b.';';
				else
					$rs .= 'border-'.$db.':1px solid transparent;';
			}
			else if($bd === 'right')
			{
				if(!$GLOBALS['columncountermax'] || ($GLOBALS['columncounter'] < $GLOBALS['columncountermax']))
					$rs .= 'border-'.$bd.':1px solid '.$b.';';
				else
					$rs .= 'border-'.$db.':1px solid transparent;';
			}
			else
				$rs .= 'border-right:1px solid transparent;';
		}
	}
	if(!$GLOBALS['SKIP_COLUMN_SPACERS'])
		$GLOBALS['columnspacer']=true;

	// Help
	if(isset($_POST['mt_hl']) && $_POST['mt_hl'])
	{
		if($width)
			$rs .= 'box-shadow:0 0 2px #006b9f inset;';
		else
			$rs .= 'box-shadow:0 0 2px #ff00ff inset;';
	}

	$add_style='maske'.$mn.'_as'.$aktzeile;
	if(isset($GLOBALS[$add_style]))
	{
		$rs .= $GLOBALS[$add_style];
//echo('<b>Add style</b> ['.$add_style.'] = &quot;'.$GLOBALS[$add_style].'&quot;<br />');
	}

	if($with_style && strlen($rs))
		$rs=' style="'.$rs.'"';

//echo('$rs='.fxHtmlEncode($rs).'<hr size=1 />');
	return $rs;
}

/**
 * get_columninfo
 * Diese Funktion öffnet ein TD-Tag mit den entspr. Eigenschaften:
 * Breite, Ausrichtung, etc.
 * -> wird für die Darstellung der Maske benötigt
 * -----------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @param various $mskfld_array - Mandatory parameter:                    ???PARAMETER???
 * @param various $aktzeile     - Mandatory parameter:                    ???PARAMETER???
 * @param string  $css_id       - Optional parameter (default = ''):      ???PARAMETER???
 * @param integer $width        - Optional parameter (default = 0):       ???PARAMETER???
 * @param string  $align        - Optional parameter (default = 'left'):  ???PARAMETER???
 * @param string  $msk_nowrap   - Optional parameter (default = ''):      ???PARAMETER???
 * @param string  $fld_tooltip  - Optional parameter (default = ''):      ???PARAMETER???
 * @param string  $add          - Optional parameter (default = ''):      ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_columninfo($mskfld_array, $aktzeile, $css_id='', $width=0, $align='left', $msk_nowrap='', $fld_tooltip='', $add='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$GLOBALS['mtd_width']=0;
//fxDebug($mskfld_array,'$mskfdl_array');

	$with_border=true;
	$nbd=$mskfld_array['feldname'].'_nbd';
	if(isset($GLOBALS[$nbd]) && $GLOBALS[$nbd])
		$with_border=false;
	$headline=false;
	if($align == 'header')
		$headline=true;
	$sp=getColumnspace($mskfld_array,$aktzeile,$headline,false,$width,$with_border);
	if($css_id == 'ro')
	{
		$sp .= 'height:24px;padding-top:1px;';
		if(isset($_POST['mt_hl']) && $_POST['mt_hl'])
			$sp .= 'box-shadow:0 0 4px #008888 inset;';
	}
	$rs=tdo().' id="td_';
//echo('<hr>td ['.$mskfld_array['feldname'].']='.$mskfld_array['anzeigeart'].'<br />');
	if($mskfld_array['anzeigeart'] < 5)
		$rs .= 'lit_';
	$rs .= $mskfld_array['feldname'];
	if(isset($GLOBALS['bodyzeilenindex']) && strlen($GLOBALS['bodyzeilenindex']))
		$rs .= $GLOBALS['bodyzeilenindex'];
	$rs .= '"';
	$with_class=true;
	$ncl=$mskfld_array['feldname'].'_ncl';
	if(isset($GLOBALS[$ncl]) && $GLOBALS[$ncl])
		$with_class=false;
	if($css_id && $with_class)
	{
		if(substr($css_id, 0, 1) == '^')		// Farb-Override CLASS
			$rs .= ' class="'.substr($css_id, 1).'"';
		else if(substr($css_id, 0, 1) == '#')	// Farb-Override COLOR
			$rs .= ' bgcolor="#'.substr($css_id, 1).'"';
		else									// ID
			$rs .= ' class="'.$css_id.'"';
	}
	if(($align == 'header') || $msk_nowrap || (((int)$mskfld_array['zusatzparameter_int1'])&1) || (($mskfld_array['anzeigeart'] > 0) && ($mskfld_array['anzeigeart'] < 4)) || ($mskfld_array['anzeigeart'] == 8) || ($mskfld_array['feldtyp'] == 11))
		$rs .= ' nowrap';
	if(fxIsArray($mskfld_array)) // Array übergeben
	{
		// Width
		if((($align != 'header') || $GLOBALS['set_header_width']) && ($width != 0))
		{
			// Width-Override
			$lw=0;
			$tw=0;
			$lwf=$mskfld_array['feldname'].'_lwidth';
			if(isset($GLOBALS[$lwf]) && $GLOBALS[$lwf] && $mskfld_array['anzeigeart'] && (($mskfld_array['anzeigeart'] < 5) || ($mskfld_array['anzeigeart'] == 20)))	// Literal
			{
				$width=$GLOBALS[$lwf];
//echo('<b>WIDTH</b> for literal field &quot;'.$mskfld_array['feldname'].'&quot; = '.$width.'<br />');
			}
			else
			{
				$twf=$mskfld_array['feldname'].'_twidth';
				if(isset($GLOBALS[$twf]) && $GLOBALS[$twf])
				{
					$width=$GLOBALS[$twf];
//echo('<b>WIDTH</b> for td field &quot;'.$mskfld_array['feldname'].'&quot; = '.$width.'<br />');
				}
			}
			if(substr($width, -1) == '%')
				$rs .= ' width="'.$width.'"';
			else
			{
				$GLOBALS['mtd_width']=(int)($width * $GLOBALS['msk_wfac']);
				$rs .= ' width="'.$GLOBALS['mtd_width'].'"';
			}
		}

		// Height
		if($align != 'header')
		{
			$height='';
			$setonce=false;
			$lwf='maske'.$mskfld_array['nummer'].'_'.$mskfld_array['feldname'].'_height_'.$aktzeile;
			if(isset($GLOBALS[$lwf]) && $GLOBALS[$lwf])
				$height=trim($GLOBALS[$lwf]);
			if(!strlen($height))
			{
				$lwf='maske'.$mskfld_array['nummer'].'_'.$mskfld_array['feldname'].'_height';
				if(isset($GLOBALS[$lwf]) && $GLOBALS[$lwf])
					$height=trim($GLOBALS[$lwf]);
			}
			if(!strlen($height))
			{
				$lwf=$mskfld_array['feldname'].'_height_'.$aktzeile;
				if(isset($GLOBALS[$lwf]) && $GLOBALS[$lwf])
					$height=trim($GLOBALS[$lwf]);
			}
			if(!strlen($height))
			{
				$lwf=$mskfld_array['feldname'].'_height';
				if(isset($GLOBALS[$lwf]) && $GLOBALS[$lwf])
					$height=trim($GLOBALS[$lwf]);
			}
			if(!strlen($height))
			{
				$lwf='maske'.$mskfld_array['nummer'].'_trheight_'.$aktzeile;
				if(isset($GLOBALS[$lwf]) && $GLOBALS[$lwf])
				{
					$lws='maske'.$mskfld_array['nummer'].'_dso_'.$aktzeile;
					if(!isset($GLOBALS[$lws]) || !$GLOBALS[$lws])
						$setonce=true;
					$height=trim($GLOBALS[$lwf]);
				}
			}
			if(!strlen($height))
			{
				$lwf='maske'.$mskfld_array['nummer'].'_trheight';
				if(isset($GLOBALS[$lwf]) && $GLOBALS[$lwf])
				{
					$lws='maske'.$mskfld_array['nummer'].'_dso';
					if(!isset($GLOBALS[$lws]) || !$GLOBALS[$lws])
						$setonce=true;
					$height=trim($GLOBALS[$lwf]);
				}
			}
			if(strlen($height) && ((int)$height > 0) && (!$setonce || !isset($GLOBALS['maske'.$mskfld_array['nummer'].'_hset_'.$aktzeile])))
			{
				$GLOBALS['maske'.$mskfld_array['nummer'].'_hset_'.$aktzeile]=true;
//echo('<b>HEIGHT</b> set for field &quot;'.$mskfld_array['feldname'].'&quot; [Zeile #'.$aktzeile.'] = '.$height.'<br />');
				$rs .= ' height="'.$height.'"';
			}
		}

		if(isset($GLOBALS[$mskfld_array['feldname'].'_align']))
			$align=trim($GLOBALS[$mskfld_array['feldname'].'_align']);

		if(($align != 'left') && ($align != 'header'))
			$rs .= ' align="'.$align.'"';
		else if($mskfld_array['horausr'] && ($mskfld_array['horausr'] != 'left'))
			$rs .= ' align="'.$mskfld_array['horausr'].'"';
		if($mskfld_array['vertausr'] && ($mskfld_array['vertausr'] != 'middle'))
			$rs .= ' valign="'.$mskfld_array['vertausr'].'"';
		else if(!$mskfld_array['vertausr'])
			$rs .= ' valign="top"';
		if(strlen($add))
			$rs .= ' '.$add;
		else if(strlen($fld_tooltip))
		{
			$trans_table=array(chr(9)=>'&nbsp;', chr(10).chr(10)=>', ', chr(10)=>', ');
			$fld_tooltip=strtr($fld_tooltip, $trans_table);
			$rs .= ' tooltip="'.$fld_tooltip.'"';
			$sp .= 'cursor:help;';
 		}
 	}
	if(strlen($sp))
		$rs .= ' style="'.$sp.'"';
	$rs .= '>';
//if($align == 'header') echo('<pre>'.fxHtmlEncode($rs).'</pre>');

	return $rs;
}

/**
 * close_td
 * Diese Funktion schließt den geöffneten TD-Tag wieder.
 * -> wird für die Darstellung der Maske benötigt
 * -----------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @param string $waehrung - Optional parameter (default = ''): ???PARAMETER???
 * @param string $colid    - Optional parameter (default = ''): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function close_td($waehrung='', $colid='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$rs=tdc();
	if($waehrung != '')
	{
		$col='';
		if($GLOBALS['negnumber'])
			$col=' red';
		$rs .= tdo().' class="'.$colid.$col.'">&nbsp;'.$waehrung.tdc().trc().'</table>'.tdc();
		$GLOBALS['negnumber']=false;
	}

	if($GLOBALS['tdopen'] > 1)
		$rs .= trc().'</table>'.tdc();

	return $rs.$GLOBALS['nl'];
}

/**
 * strip_help_tags
 * Diese Funktion entfernt und disabled alle HTML-Tags
 * -----------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @param various $ts - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function strip_help_tags($ts)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Step 1: Alle Links entfernen
	$cts= $ts;
	$cts_up= strtoupper($cts);
	while(true)
	{
		$lss=strpos($cts_up, '<A ');
		if($lss === false)
			break;
		$lse=strpos($cts_up, '/A>', $lss);
		$cts= substr($cts, 0, $lss).substr($cts, $lse+3);
		$cts_up= strtoupper($cts);
	}

	// Step 2: Ersten Tag disablen
	$lss=strpos($cts, '>');
	if($lss !== false)
		$cts=substr($cts, 0, $lss).' DISABLED'.substr($cts, $lss);

//Debug-Ausgaben:
//echo('<B>Vorher:</B><br /><FONT COLOR="#880000">'.fxHtmlEncode($ts).'</FONT><br />');
//echo('<B>Nachher:</B><br /><FONT COLOR="#008800">'.fxHtmlEncode($cts).'</FONT><br />');

	return $cts;
}

/**
 * ???FUNCTION???
 *
 * @param various $err_id  - Mandatory parameter: ???PARAMETER???
 * @param various $err_mnr - Mandatory parameter: ???PARAMETER???
 * @param various $err_nr  - Mandatory parameter: ???PARAMETER???
 * @param various $err_txt - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function maskErrorTooltip($err_id, $err_mnr, $err_nr, $err_txt)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$err_id)
		return '';

	if(!fxIsArray($GLOBALS['mskttfhl']) || !isset($GLOBALS['mskttfhl'][$err_mnr]))
		$GLOBALS['mskttfhl'][$err_mnr]=meldung($err_mnr);

	$ec=errorCol($err_id);
	return ' tooltip="<font color=#'.$ec['col'].'>'.$err_txt.'</font>"';
}

/**
 * Get mask filename
 *
 * @param string  $mnr - Optional parameter (default = ''): Maks number
 * @param integer $lng - Optional parameter (default = 0):  Language Id, if 0 the default language is used
 *
 * @return Mask filename
 */
function maske_filename($mnr='', $lng=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($mnr))
		$mnr=$GLOBALS['_masknumber'];
	else
		$mnr=(string)(trim($mnr));

	$lng=(int)$lng;
	if($lng <= 0)
		$lng=$GLOBALS['fxpglobals']['lang'];

	// Dateiname ermitteln anhand der Maskennummer
	$msk_filename=strtolower($mnr);
	if(substr($msk_filename, 0, 1) == '-')
		$msk_filename='u'.substr('000000'.substr($msk_filename, 1), -6);
	else if(substr($msk_filename, 0, 1) == 'r')
		$msk_filename='r'.substr('000000'.substr($msk_filename, 1), -6);
	else
		$msk_filename='m'.substr('000000'.$msk_filename, -6);
	if(isset($GLOBALS['micpath']))
		$msk_filename=$GLOBALS['micpath'].$msk_filename.'_'.$lng.'.pfi';
	else
		$msk_filename=$GLOBALS['datpath'].$msk_filename.'_'.$lng.'.pfi';

	return $msk_filename;
}

/**
 * ???FUNCTION???
 *
 * @param boolean $all - Optional parameter (default = false): ???PARAMETER???
 */
function clearOldSessionMasks($all=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sql="SELECT nummer FROM trans_maske WHERE transakt_id=".$GLOBALS['fxpglobals']['tr'];
	$arr=db_values($sql,'*');
	if($all || !fxIsArray($arr) || !fxIsArray($GLOBALS['fx_mask']))
	{
		$GLOBALS['fx_mask']			= array();
		$GLOBALS['fx_mfields']		= array();
		$GLOBALS['fx_mfieldsarray']	= array();
		$GLOBALS['fx_mheaders']		= array();
		return;
	}

	foreach($GLOBALS['fx_mask'] as $fxmnr => $dummy)
	{
		$clear=true;
		foreach($arr as $a)
		{
			$mnr=(string)$a['nummer'];
			if($fxmnr == $mnr)
			{
				$clear=false;
				break;
			}
		}
		if($clear)
		{
			unset($GLOBALS['fx_mask'][$fxmnr]);
			unset($GLOBALS['fx_mfields'][$fxmnr]);
			unset($GLOBALS['fx_mfieldsarray'][$fxmnr]);
			unset($GLOBALS['fx_mheaders'][$fxmnr]);
		}
	}
}

/**
 * ???FUNCTION???
 *
 * @param string  $mnr      - Optional parameter (default = ''):      ???PARAMETER???
 * @param boolean $repgen   - Optional parameter (default = false):   ???PARAMETER???
 * @param string  $reptitle - Optional parameter (default = ''):      ???PARAMETER???
 * @param string  $repparse - Optional parameter (default = ''):      ???PARAMETER???
 * @param array   $mskfld   - Optional parameter (default = array()): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function sessionMask($mnr='', $repgen=false, $reptitle='', $repparse='', $mskfld=array())
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($mnr))
		$mnr=(string)$GLOBALS['_masknumber'];
	else
		$mnr=(string)(trim($mnr));
//echo('<b>sessionMask</b>($mnr='.$mnr.')<hr />');

	// Prüfen, ob Maskeninfos bereits eingeladen
	if(!fxIsArray($mskfld) && fxIsArray($GLOBALS['fx_mask']) && fxIsArray($GLOBALS['fx_mask'][$mnr]) && fxIsArray($GLOBALS['fx_mfields']) && fxIsArray($GLOBALS['fx_mfields'][$mnr]) && fxIsArray($GLOBALS['fx_mfieldsarray']) && fxIsArray($GLOBALS['fx_mfieldsarray'][$mnr]))
		return false;

	// Standardstringreplace
	$mic_replace=array("'" => "\\'", "\\" => '\\\\');

	// Dateiname ermitteln anhand der Maskennummer
	$msk_filename=maske_filename($mnr);
//echo('<b>$msk_filename</b> = '.$msk_filename.'<hr />');

	// Werte löschen
	if(!fxIsArray($GLOBALS['fx_mask']))
		$GLOBALS['fx_mask']=array();
	$GLOBALS['fx_mask'][$mnr]=array();

	if(!fxIsArray($GLOBALS['fx_mfields']))
		$GLOBALS['fx_mfields']=array();
	$GLOBALS['fx_mfields'][$mnr]=array();

	if(!fxIsArray($GLOBALS['fx_mfieldsarray']))
		$GLOBALS['fx_mfieldsarray']=array();
	$GLOBALS['fx_mfieldsarray'][$mnr]=array();

	if(!fxIsArray($GLOBALS['fx_mheaders']))
		$GLOBALS['fx_mheaders']=array();
	$GLOBALS['fx_mheaders'][$mnr]=array();

	// Prüfen, ob Maskeninfos schon als Datei abgespeichert
	if(!fxIsArray($mskfld) && file_exists($msk_filename))
	{
		require($msk_filename);

		$GLOBALS['fx_mask'][$mnr]=$GLOBALS['tmask'];
		$GLOBALS['fx_mfields'][$mnr]=$GLOBALS['tmfields'];
		$GLOBALS['fx_mfieldsarray'][$mnr]=$GLOBALS['tmfieldsarray'];
		$GLOBALS['fx_mheaders'][$mnr]=$GLOBALS['tmheaders'];
	}
	else
	{
		$file_txt='<'.'?'.'php'.$GLOBALS['nl_b'];

		// Maskeninformationen einlesen und prüfen, ob Maske existiert
		$umnr=substr(strtoupper($GLOBALS['_masknumber']),0,1);
		if(!$repgen && ($umnr == 'R'))
			$repgen=true;
		if($repgen) // Maske für Reportgenerator erstellen
		{
			$GLOBALS['fx_mask'][$mnr]=array(
				'nummer'		=> $mnr,
				'listenansicht'	=> 1,
				'maskenart'		=> 4,
				'status_maske'	=> 5,
				'ausrichtung'	=> 'left',
				'text_kurz'		=> $reptitle
			);
		}
		else
		{
			$sql  = "SELECT m.name_maske, m.breite, m.listenansicht, m.maskenart, m.ausrichtung, m.border, m.auswahlmaske, m.status_maske, t.text_kurz, t.text_varlang, t.literal_20";
			$sql .= " FROM masken m, texte t";
			$sql .= " WHERE t.id_referenz=m.nummer AND m.nummer=".(int)$mnr." AND m.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].") AND t.id_ref_art=2 AND t.id_sprache=".$GLOBALS['fxpglobals']['lang'];

			$GLOBALS['fx_mask'][$mnr]=db_values($sql);
		}
//fxDebug($GLOBALS['fx_mask'][$mnr],'$GLOBALS[\'fx_mask\']['.$mnr.']', 0);

		// Maske abspeichern
		if(fxIsArray($GLOBALS['fx_mask'][$mnr]))
		{
			$tmp_file_txt='$GLOBALS[\'tmask\']=array'.$GLOBALS['nl_b'].'('.$GLOBALS['nl_b'];
			foreach($GLOBALS['fx_mask'][$mnr] as $sm_lit => $sm_val)
				$tmp_file_txt .= '	\''.$sm_lit.'\' => \''.strtr($sm_val, $mic_replace).'\','.$GLOBALS['nl_b'];
			$tmp_file_txt .= ');'.$GLOBALS['nl_b'].$GLOBALS['nl_b'];

			$file_txt .= $tmp_file_txt;
		}
		// Maske mit der entsprechenden Nummer gibt es nicht
		else
		{
			if(isset($GLOBALS['fxpvars']['lookups'][$mnr]))
				unset($GLOBALS['fxpvars']['lookups'][$mnr]);
			$GLOBALS['_maskcounter']++;
			return true;
		}

		// Alle Maskenfelder einlesen und sortieren
		if(!fxIsArray($mskfld))
		{
			$mskfld=array();
			$anzart=array();
			if($repgen) // Maskenfelder für Reportgenerator erstellen
			{
				if(fxIsArray($repparse) && fxIsArray($repparse['fld']))
				{
					foreach($repparse['fld'] as $k => $fld)
					{
						if($fld['summe'])		// "Sum"
							$anzart[$k]=14;
						else
							$anzart[$k]=5;

						$mskfld[$k]=array(
							'nummer'			=> $mnr,
							'zeile'				=> 1,
							'spalte'			=> $k,
							'anzeigeart'		=> $anzart[$k],	// "Read-Only" or "Sum"
							'eingabefeld'		=> $fld['id'],
							'anzeigegroesse1'	=> 3,			// Use "Literal-15" by default
							'horausr'			=> 'left',
							'defaultwert'		=> 1
						);

						// Right-align number fields, except LookUp-Values
						$ift=$fld['typ'];
						if(($ift == 5) || ($ift == 6) || ($ift == 11) || ($ift == 12) || ($ift == 14))
							$mskfld[$k]['horausr']='right';
					}
				}
			}
			else
			{
				$sfl  = "nummer, maskenfeldid, zeile, spalte, anzeigeart, eingabefeld, anzeigegroesse1, anzeigegroesse2, muss, horausr, vertausr,";
				$sfl .= " spaltenanzahl, zeilenanzahl, spaltenbreite, zeilenhoehe, literal_overwrite, NULL AS literal_value, defaultwert, spezial,";
				$sfl .= " idintab, zusatzparameter_int1, zusatzparameter_int2, zusatzparameter_txt";

				$sql  = "SELECT ".$sfl." FROM maskenfelder WHERE nummer=".(int)$mnr." ORDER BY zeile, spalte, zeilenanzahl, spaltenanzahl DESC";

				$mskfld=db_values($sql, '*');
			}

			$mcnt=0;
			if(fxIsArray($mskfld))
				$msize=sizeof($mskfld);
			else
				$msize=0;

			// Check each mask field if its an alias field
//fxDebug($mskfld,'$mskfld: $msize='.$msize, 0);
			while($mcnt < $msize)
			{
				$mskfld[$mcnt]=maske_text($mskfld[$mcnt]);
				$mcnt++;
			}
		}
		$GLOBALS['fx_mfields'][$mnr]=$mskfld;
//fxDebug($GLOBALS['fx_mfields'][$mnr],'$GLOBALS[\'fx_mfields\']['.$mnr.']', 0);

		// Zur korrekten Darstellung der Masken wird die Anzahl der Zeilen und Spalten ermittelt
		// und in einem Array zwischengespeichert, danach sortiert
		$anz_zeilen=0;
		$anz_spalten=0;
		$g=0;
		$gend=1+sizeof($GLOBALS['fx_mfields'][$mnr]);
		while($g < $gend)
		{
			if($GLOBALS['fx_mfields'][$mnr][$g] == '')
			{
				$g++;
				continue;
			}
			// Zeilen prüfen
			for($zsc=0; $zsc<$anz_zeilen; $zsc++) // nur unterschiedliche Zeilennummern im Array speichern
			{
				if($zeilenindex[$zsc] == $GLOBALS['fx_mfields'][$mnr][$g]['zeile'])
					$zsc=$anz_zeilen+1;
			}
			if($zsc == $anz_zeilen) // neuen Zeilenindex gefunden, deshalb in Array speichern
				$zeilenindex[$anz_zeilen++]=$GLOBALS['fx_mfields'][$mnr][$g]['zeile'];

			// Spalten prüfen
			for($zsc=0; $zsc < $anz_spalten; $zsc++) // nur unterschiedliche Spaltennummern im Array speichern
			{
				if($spaltenindex[$zsc] == $GLOBALS['fx_mfields'][$mnr][$g]['spalte'])
					$zsc=$anz_spalten+1;
			}
			if($zsc == $anz_spalten) // neuen Spaltenindex gefunden, deshalb in Array speichern
				$spaltenindex[$anz_spalten++]=$GLOBALS['fx_mfields'][$mnr][$g]['spalte'];

			$g++;
		}

		// Zeilen und Spaltenindizes sortieren, wenn existent
		if($zeilenindex != '')
			sort($zeilenindex);
		if($spaltenindex != '')
			sort($spaltenindex);
		$GLOBALS['fx_mfieldsarray'][$mnr]['rows']=$zeilenindex;
		$GLOBALS['fx_mfieldsarray'][$mnr]['columns']=$spaltenindex;

		// alle Felder vorab auf 'blank' setzen
		for($lnr=0; $lnr < $anz_zeilen; $lnr++)
		{
			$GLOBALS['fx_mfieldsarray'][$mnr]['height'][$zeilenindex[$lnr]]=0;
			for($cnr=0; $cnr < $anz_spalten; $cnr++)
			{
				if($lnr == 0)
					$GLOBALS['fx_mfieldsarray'][$mnr]['width'][$spaltenindex[$cnr]]=0;
				$GLOBALS['fx_mfieldsarray'][$mnr][$zeilenindex[$lnr]][$spaltenindex[$cnr]]='blank';
			}
		}

		// Maskenfelder abspeichern
		if(fxIsArray($GLOBALS['fx_mfields'][$mnr]))
		{
			$tmp_file_txt='$GLOBALS[\'tmfields\']=array'.$GLOBALS['nl_b'].'('.$GLOBALS['nl_b'];
			foreach($GLOBALS['fx_mfields'][$mnr] as $sm_nr => $sm_arr)
			{
				$tmp_file_txt .= '	'.$sm_nr.' => array'.$GLOBALS['nl_b'].'	('.$GLOBALS['nl_b'];
				if(fxIsArray($sm_arr))
				{
					foreach($sm_arr as $sm_lit => $sm_val)
						$tmp_file_txt .= '		\''.$sm_lit.'\' => \''.strtr($sm_val, $mic_replace).'\','.$GLOBALS['nl_b'];
				}
				$tmp_file_txt .= '	),'.$GLOBALS['nl_b'];
			}
			$tmp_file_txt .= ');'.$GLOBALS['nl_b'].$GLOBALS['nl_b'];
			$file_txt .= $tmp_file_txt;
		}
		else
			$file_txt .= '$GLOBALS[\'tmfields\']=array();'.$GLOBALS['nl_b'].$GLOBALS['nl_b'];

		if(fxIsArray($GLOBALS['fx_mfieldsarray'][$mnr]))
		{
			$tmp_file_txt='$GLOBALS[\'tmfieldsarray\']=array'.$GLOBALS['nl_b'].'('.$GLOBALS['nl_b'];
			foreach($GLOBALS['fx_mfieldsarray'][$mnr] as $sm_nr => $sm_arr)
			{
				$tmp_file_txt .= '	\''.$sm_nr.'\' => array'.$GLOBALS['nl_b'].'	('.$GLOBALS['nl_b'];
				if(fxIsArray($sm_arr))
				{
					foreach($sm_arr as $sm_lit => $sm_val)
						$tmp_file_txt .= '		\''.$sm_lit.'\' => \''.strtr($sm_val, $mic_replace).'\','.$GLOBALS['nl_b'];
				}
				$tmp_file_txt .= '	),'.$GLOBALS['nl_b'];
			}
			$tmp_file_txt .= ');'.$GLOBALS['nl_b'].$GLOBALS['nl_b'];
			$file_txt .= $tmp_file_txt;
		}
		else
			$file_txt .= '$GLOBALS[\'tmfieldsarray\']=array();'.$GLOBALS['nl_b'].$GLOBALS['nl_b'];

		// Maskenheader
		$file_txt .= '$GLOBALS[\'tmheaders\']=array();'.$GLOBALS['nl_b'].'?'.'>';

		fxSave($msk_filename, $file_txt, true);
	}

	return false;
}

/**
 * ???FUNCTION???
 *
 * @param string  $mnr     - Optional parameter (default = ''):    ???PARAMETER???
 * @param boolean $headers - Optional parameter (default = false): ???PARAMETER???
 */
function saveSessionHeaders($mnr='', $headers=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($mnr))
		$mnr=(string)$GLOBALS['_masknumber'];
	else
		$mnr=(string)(trim($mnr));

	// Standardstringreplace
	$mic_replace=array("'" => "\\'", "\\" => '\\\\', "&nbsp;"=>'');

	// Dateiname ermitteln anhand der Maskennummer
	$msk_filename=maske_filename($mnr);
//fxDebug($headers, $mnr.': headers -- filename='.$msk_filename);

	// Maskendatei laden
	$content=fxLoad($msk_filename);
	if($content !== false)
	{
		$hsp=strpos($content,'$GLOBALS[\'tmheaders\']=array(');
		$hep=false;
		if($hsp !== false)
			$hep=strpos($content,');'.$GLOBALS['nl_b'],$hsp+27);
		if($hep !== false)
		{
			$h=$GLOBALS['nl_b'].$GLOBALS['nl_b'].'$GLOBALS[\'tmheaders\']=array('.$GLOBALS['nl_b'];
			if(fxIsArray($headers))
			{
				foreach($headers as $c => $txt)
					$h .= '	'.$c.' => \''.strtr($txt, $mic_replace).'\','.$GLOBALS['nl_b'];
			}
			$h .= ');'.$GLOBALS['nl_b'];

			$ncontent=trim(substr($content,0,$hsp)).$h.trim(substr($content,$hep+2));
//echo('<pre>'.fxHtmlEncode($ncontent).'</pre>');

			fxSave($msk_filename, $ncontent, true);
		}
	}
}

/**
 * ???FUNCTION???
 *
 * @param integer $mtr - Optional parameter (default = 0): ???PARAMETER???
 */
function loadTransactionMasks($mtr=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$mtr)
		$mtr=$GLOBALS['fxpglobals']['tr'];

	if(fxIsArray($GLOBALS['fxptdata']['marray']) && fxIsArray($GLOBALS['fxptdata']['marray'][$mtr]))
	{
		foreach($GLOBALS['fxptdata']['marray'][$mtr] as $marr)
		{
			$mnr=$marr['_masknumber'];
			sessionMask($mnr);
		}
	}
}

/**
 * ???FUNCTION???
 *
 * @param various $mskarr - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function maske_text($mskarr)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!fxIsArray($mskarr))
		return $mskarr;

	if(strlen($mskarr['eingabefeld']))
	{
		$fld_txt=mskField((int)$mskarr['eingabefeld'],true);
		$mskarr=array_merge($mskarr,$fld_txt);

		$lit=getTRHelpLiteral($mskarr);
		if(substr($lit,0,1) == '*')
			$lit=substr($lit,1);
		else if(strlen($lit))
			$lit=$mskarr[$lit];

		if(strlen($lit))
			$mskarr['literal_value']=$lit;
	}

	if($mskarr['repeval'])
		$mskarr['nachschlage']=1;

	return $mskarr;
}

/**
 * ???FUNCTION???
 *
 * @param various $field     - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $with_text - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function mskField($field, $with_text=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Check, if table felder exists
	$fex=db_check('felder', 't');
	if(!$fex)
		return $field;

	// Field- and text values
	$sfl  = "mandanten_id, id, feldname, alias_zu, feldtyp, laenge, nachkomma, html_grafik, texte_leer,";
	$sfl .= " nachschlage, nachschlage_art, kosten";
	$stx  = "id_text, text_kurz, text_varlang, anmerkungintern AS anmerkung, literal_5, literal_10, literal_15, literal_20, literal_text, literal_tr, tooltip_tr";

	// Search for id or name
	$tfield=trim($field);
	$ifield=(int)$tfield;

	// Field already stored?
	if(fxIsArray($GLOBALS['_fields']) && fxIsArray($GLOBALS['_fields'][$tfield]))
		return $GLOBALS['_fields'][$tfield];

	// Get field values
	$sql="SELECT ".$sfl." FROM felder WHERE ";
	if($ifield != 0)
		$sql .= "id=".$ifield;
	else
		$sql .= db_like('feldname', $tfield);
	$sql .= " AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
	$fld=db_values($sql);
//fxDebug($fld,'$fld<br />'.$sql, 0);
	$txt=array();

	if(fxIsArray($fld))
	{
		$tfield=$fld['feldname'];
		$ifield=$fld['id'];

		// Alias field?
		$alias=$fld['alias_zu'];
		// ...Yes
		if($alias != 0)
		{
			if($with_text)
			{
				$tempty=$fld['texte_leer'];

				// Get text values
				$idref=$fld['id'];
				if($tempty != 0)
					$idref=$alias;
				$sql="SELECT ".$stx." FROM texte WHERE id_referenz=".$idref." AND id_ref_art=3 AND id_sprache=".$GLOBALS['fxpglobals']['lang'];
				$txt=db_values($sql);
//fxDebug($txt,$sql, 0);
			}

			// Reload field values for alias
			$sql="SELECT ".$sfl." FROM felder WHERE id=".$alias." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
			$fld=db_values($sql);
//fxDebug($fld,$sql, 0);

			$fld['feldname']=$tfield;
		}
		// ...No
		else if($with_text)
		{
			$sql="SELECT ".$stx." FROM texte WHERE id_referenz=".$fld['id']." AND id_ref_art=3 AND id_sprache=".$GLOBALS['fxpglobals']['lang'];
			$txt=db_values($sql);
//fxDebug($txt,$sql, 0);
		}
	}

	$ret=$fld;
	if(fxIsArray($txt))
		$ret=array_merge($ret, $txt);

	if(!fxIsArray($GLOBALS['_fields']))
		$GLOBALS['_fields']=array();
	$GLOBALS['_fields'][$tfield]=$ret;

	// Add field to mask if not found
	addMaskField($ifield);

	return $ret;
}

/**
 * ???FUNCTION???
 *
 * @param various $ifield - Mandatory parameter:                ???PARAMETER???
 * @param string  $mnr    - Optional parameter (default = ''):  ???PARAMETER???
 */
function addMaskField($ifield, $mnr='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Add field to mask if not found
	$ifield=(int)$ifield;
	$mnr=trim($mnr);
	if(!strlen($mnr))
		$mnr=$GLOBALS['_masknumber'];
	if(fxIsArray($GLOBALS['fx_mfields']) && fxIsArray($GLOBALS['fx_mfields'][$mnr]))
	{
		$lcnt=-1;
		$fpos=-1;
		foreach($GLOBALS['fx_mfields'][$mnr] as $cnt => $farr)
		{
			$icnt=(int)$cnt;
			if((string)$icnt == $cnt)
			{
				if($farr['id'] == $ifield)
				{
					$fpos=$cnt;
					break;
				}
				$lcnt=$icnt;
			}
		}
		if($fpos < 0)
		{
			$lcnt++;
			$GLOBALS['fx_mfields'][$mnr][$lcnt]=array(
				'id'=> $ifield,
				'anzeigeart'=> 99
			);
		}
	}
}

/**
 * ???FUNCTION???
 *
 * @param various $value - Mandatory parameter:                   ???PARAMETER???
 * @param various $type  - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $sess  - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fieldValue($value, $type, $sess=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sec='';
	$hidden=false;
	if($type == 'hidden')
		$hidden=true;
	if(!is_array($value) && strlen($value))
	{
		// Check if it is a timespan value
		$tsp=strpos($value,'id=tshd_');
		if($tsp && ($type == 'text'))
		{
			// Yes, get seconds...
			$ses=strpos($value,' sec=',$tsp);
			$see=strpos($value,'>',$ses+5);
			$sec=substr($value,$ses,$see-$ses);
			// ...and remove tags and extract only the value
			$etp=strpos($value,'>',$tsp);
			$stp=strpos($value,'<',$etp);
			$rvalue=substr($value,$etp+1,$stp-$etp-1);
		}
		else
			$rvalue=$value;
	}
	else
		$rvalue='';

	if($sess)	// Value for session, then just real value
		$tmp=$rvalue;
	else
		$tmp=$sec.' value="'.$rvalue.'"';
//echo('$value=['.fxHtmlEncode($value).'] - $tmp=['.fxHtmlEncode($tmp).']<br />');

	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various $fieldname - Mandatory parameter:                ???PARAMETER???
 * @param various $value     - Mandatory parameter:                ???PARAMETER???
 * @param string  $fid       - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $form      - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fieldHidden($fieldname, $value, $fid='', $form='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(strlen($form))
		setForm($form);

	$findex='';
	$idp=strpos($fieldname,'[');
	if($idp)
	{
		$findex=substr($fieldname,$idp);
		$fieldname=substr($fieldname,0,$idp);
	}
	if(!strlen($fid))
		$fid=$fieldname.$findex;
	$tmp='<input type="hidden" id="'.$fid.'" name="'.$fieldname.$findex.'" fxform="'.$GLOBALS['_form'].'"'.fieldValue($value,'hidden').'>';
//echo('<pre>'.fxHtmlEncode($tmp).'</pre>');

	// Save field info
	if(substr($fieldname,0,5) != 'temp_')
		fxSession('t','p',$fieldname.$findex,$value);

	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various $fieldname - Mandatory parameter:                    ???PARAMETER???
 * @param various $value     - Mandatory parameter:                    ???PARAMETER???
 * @param integer $width     - Optional parameter (default = 0):       ???PARAMETER???
 * @param integer $len       - Optional parameter (default = 0):       ???PARAMETER???
 * @param string  $align     - Optional parameter (default = 'left'):  ???PARAMETER???
 * @param boolean $mand      - Optional parameter (default = false):   ???PARAMETER???
 * @param boolean $dis       - Optional parameter (default = false):   ???PARAMETER???
 * @param boolean $nva       - Optional parameter (default = false):   ???PARAMETER???
 * @param string  $tooltip   - Optional parameter (default = ''):      ???PARAMETER???
 * @param string  $jsfunc    - Optional parameter (default = ''):      ???PARAMETER???
 * @param string  $ftype     - Optional parameter (default = 'text'):  ???PARAMETER???
 * @param string  $form      - Optional parameter (default = ''):      ???PARAMETER???
 * @param string  $style_add - Optional parameter (default = ''):      ???PARAMETER???
 * @param string  $class_add - Optional parameter (default = ''):      ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fieldText($fieldname, $value, $width=0, $len=0, $align='left', $mand=false, $dis=false, $nva=false, $tooltip='', $jsfunc='', $ftype='text', $form='', $style_add='', $class_add='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(strlen($form))
		setForm($form);

	$findex='';
	$idp=strpos($fieldname,'[');
	if($idp)
	{
		$findex=substr($fieldname,$idp);
		$fieldname=substr($fieldname,0,$idp);
	}

	$fvalues=mskField($fieldname);
	$errcode=errorCode($fieldname,$GLOBALS['_masknumber'],$findex);

	$style=$errcode['style'];
	$crs='';

	if($dis === 'hide')
	{
		$dis=false;
		$style .= 'display:none;';
	}

	if(strlen($style_add))
		$style .= $style_add;
	if(isset($GLOBALS[$fieldname.'_style'.$findex]))
		$style .= $GLOBALS[$fieldname.'_style'.$findex];

	if(isset($GLOBALS[$fieldname.'_tooltip'.$findex]))
		$tooltip=$GLOBALS[$fieldname.'_tooltip'.$findex];

	if($width)
		$width=(int)($width * $GLOBALS['msk_wfac']);

	if(isset($GLOBALS[$fieldname.'_len'.$findex]))
		$len=(int)$GLOBALS[$fieldname.'_len'.$findex];
	else if(isset($GLOBALS[$fieldname.'_len']))
		$len=(int)$GLOBALS[$fieldname.'_len'];

	$pftype=strtolower(substr($ftype,0,3));
	$attr='';
	if(($pftype != 'pas') && ($pftype != 'fil') && ($pftype != 'col'))
	{
		$ftype='text';
		if(($pftype == 'tex') && $GLOBALS['fxpglobals']['tr'] && ($GLOBALS['fxpglobals']['tr'] != 83) && ($GLOBALS['fxpglobals']['tr'] != 189) && (!$len || ($len > 32)))
			$attr=' enlarge';
	}

	$mfindex='';
	$multiple='';
	if(($pftype == 'fil') && (substr($ftype,-1) == 'm'))
	{
		$ftype='file';
		$mfindex='[]';
		$multiple=' multiple="multiple"';
	}

	$tmp='';
	if($dis && ($dis !== 'yes'))
		$tmp .= fieldHidden($fieldname.$findex,$value);
	$tmp .= '<input class="fxf';
	if($pftype == 'pas')
		$fclass='pw';
	else if($pftype == 'fil')
		$fclass='tf';
	else if($pftype == 'cod')
		$fclass='tc';
	else
		$fclass='tx';
	$tmp .= $fclass;
	if($mand)
		$tmp .= 'm';
	$tabindex=0;
	if($nva || $dis)
	{
		$tmp .= 'g';
		$tabindex=-1;
	}
	if(strlen($class_add))
		$tmp .= ' '.$class_add;
	if(isset($GLOBALS[$fieldname.'_class'.$findex]))
		$tmp .= ' '.$GLOBALS[$fieldname.'_class'.$findex];
	$tmp .= '" type="'.$ftype.'"'.$multiple.' id="'.$fieldname.$findex.'" name="'.$fieldname.$findex.$mfindex.'" tabindex="'.$tabindex.'" fxform="'.$GLOBALS['_form'].'" ref="fx_tx_'.$GLOBALS['_masknumber'].'"'.$attr;
	if($pftype != 'fil')
		$tmp .= fieldValue($value,$ftype);
	if($len)
		$tmp .= ' maxlength="'.$len.'"';
	if($width)
		$style .= 'width:'.$width.'px;';
	else
		$style .= 'width:100%;';
	if($nva || $dis)
		$tmp .= ' readonly="readonly"';
	if($dis)
	{
		$style .= 'color:#888888;';
		$udf=$fieldname.'_ud'.$findex;
		if(isset($GLOBALS[$udf]) && $GLOBALS[$udf])
			$tmp .= ' ud="1"';
		else
			$tmp .= ' disabled';
	}
	if($errcode['id'])
		$tmp .= maskErrorTooltip($errcode['id'], $errcode['mnr'], $errcode['nr'], $errcode['txt']);
	else if(strlen($tooltip))
	{
		$crs = 'help';
		$tmp .= ' tooltip="'.$tooltip.'"';
	}
	if(strlen($align) && ($align != 'left'))
		$style .= 'text-align:'.$align.';';
	if($pftype == 'fil')
		$crs = 'default';
	if(strlen($jsfunc) && (substr(trim($jsfunc),0,8) != 'onchange'))
		$crs = 'pointer';
	if(strlen($crs))
		$style .= 'cursor:'.$crs.';';
	if(strlen($style))
		$tmp .= ' style="'.$style.'"';
	if(strlen($jsfunc))
		$tmp .= ' '.$jsfunc;
	$tmp .= '>';
//echo('<pre>'.fxHtmlEncode($tmp).'</pre>');

	// Save field info
	if(!$dis && ($pftype != 'fil'))
	{
		$fvalue=fieldValue($value,$ftype,true);
		if(($fclass == 'pw') || ($fclass == 'tc'))
			$fvalue=fxEncodeValue($fvalue);
		if(substr($fieldname,0,5) != 'temp_')
			fxSession('t','p',$fieldname.$findex,$fvalue);
	}

	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various $fieldname - Mandatory parameter:                   ???PARAMETER???
 * @param various $value     - Mandatory parameter:                   ???PARAMETER???
 * @param integer $width     - Optional parameter (default = 0):      ???PARAMETER???
 * @param integer $len       - Optional parameter (default = 0):      ???PARAMETER???
 * @param boolean $mand      - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $dis       - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $nva       - Optional parameter (default = false):  ???PARAMETER???
 * @param string  $tooltip   - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $jsfunc    - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $form      - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $style_add - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $class_add - Optional parameter (default = ''):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fieldPassword($fieldname, $value, $width=0, $len=0, $mand=false, $dis=false, $nva=false, $tooltip='', $jsfunc='', $form='', $style_add='', $class_add='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return fieldText($fieldname, fxDecodeValue($value), $width, $len, 'left', $mand, $dis, $nva, $tooltip, $jsfunc, 'password', $form, $style_add, $class_add);
}

/**
 * ???FUNCTION???
 *
 * @param various $fieldname - Mandatory parameter:                   ???PARAMETER???
 * @param various $value     - Mandatory parameter:                   ???PARAMETER???
 * @param integer $width     - Optional parameter (default = 0):      ???PARAMETER???
 * @param integer $len       - Optional parameter (default = 0):      ???PARAMETER???
 * @param boolean $mand      - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $dis       - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $nva       - Optional parameter (default = false):  ???PARAMETER???
 * @param string  $tooltip   - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $jsfunc    - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $form      - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $style_add - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $class_add - Optional parameter (default = ''):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fieldCode($fieldname, $value, $width=0, $len=0, $mand=false, $dis=false, $nva=false, $tooltip='', $jsfunc='', $form='', $style_add='', $class_add='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return fieldText($fieldname, fxDecodeValue($value), $width, $len, 'left', $mand, $dis, $nva, $tooltip, $jsfunc, 'code', $form, $style_add, $class_add);
}

/**
 * ???FUNCTION???
 *
 * @param various $fieldname - Mandatory parameter:                   ???PARAMETER???
 * @param various $value     - Mandatory parameter:                   ???PARAMETER???
 * @param integer $width     - Optional parameter (default = 0):      ???PARAMETER???
 * @param boolean $mand      - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $dis       - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $nva       - Optional parameter (default = false):  ???PARAMETER???
 * @param string  $tooltip   - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $jsfunc    - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $form      - Optional parameter (default = ''):     ???PARAMETER???
 * @param boolean $setcol    - Optional parameter (default = false):  ???PARAMETER???
 * @param string  $style_add - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $class_add - Optional parameter (default = ''):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fieldInteger($fieldname, $value, $width=0, $mand=false, $dis=false, $nva=false, $tooltip='', $jsfunc='', $form='', $setcol=false, $style_add='', $class_add='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($setcol)
	{
		if($value < 0.00)
			$style_add .= 'color:#e1001a;';
		else
			$style_add .= 'color:#000000;';
	}
	$value=get_entry($value, 'ganzzahl');

	return fieldText($fieldname, $value, $width, 20, 'right', $mand, $dis, $nva, $tooltip, $jsfunc, 'integer', $form, $style_add, $class_add);
}

/**
 * ???FUNCTION???
 *
 * @param various $fieldname - Mandatory parameter:                   ???PARAMETER???
 * @param various $value     - Mandatory parameter:                   ???PARAMETER???
 * @param integer $width     - Optional parameter (default = 0):      ???PARAMETER???
 * @param boolean $mand      - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $dis       - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $nva       - Optional parameter (default = false):  ???PARAMETER???
 * @param string  $tooltip   - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $jsfunc    - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $form      - Optional parameter (default = ''):     ???PARAMETER???
 * @param boolean $setcol    - Optional parameter (default = false):  ???PARAMETER???
 * @param string  $style_add - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $class_add - Optional parameter (default = ''):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fieldFloat($fieldname, $value, $width=0, $mand=false, $dis=false, $nva=false, $tooltip='', $jsfunc='', $form='', $setcol=false, $style_add='', $class_add='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($setcol)
	{
		if($value < 0.00)
			$style_add .= 'color:#e1001a;';
		else
			$style_add .= 'color:#000000;';
	}
	$value=get_entry($value, 'dezimal');

	return fieldText($fieldname, $value, $width, 25, 'right', $mand, $dis, $nva, $tooltip, $jsfunc, 'float', $form, $style_add, $class_add);
}

/**
 * ???FUNCTION???
 *
 * @param various $fieldname - Mandatory parameter:                   ???PARAMETER???
 * @param various $value     - Mandatory parameter:                   ???PARAMETER???
 * @param integer $width     - Optional parameter (default = 0):      ???PARAMETER???
 * @param boolean $mand      - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $dis       - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $nva       - Optional parameter (default = false):  ???PARAMETER???
 * @param string  $tooltip   - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $jsfunc    - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $form      - Optional parameter (default = ''):     ???PARAMETER???
 * @param boolean $setcol    - Optional parameter (default = false):  ???PARAMETER???
 * @param string  $style_add - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $class_add - Optional parameter (default = ''):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fieldTimespan($fieldname, $value, $width=0, $mand=false, $dis=false, $nva=false, $tooltip='', $jsfunc='', $form='', $setcol=false, $style_add='', $class_add='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($setcol)
	{
		if($value < 0.00)
			$style_add .= 'color:#e1001a;';
		else
			$style_add .= 'color:#000000;';
	}
	$value=get_entry($value, 'zeitspanne');
	return fieldText($fieldname, $value, $width, 25, 'right', $mand, $dis, $nva, $tooltip, $jsfunc, 'timespan', $form, $style_add, $class_add);
}

/**
 * ???FUNCTION???
 *
 * @param various $fieldname - Mandatory parameter:                   ???PARAMETER???
 * @param various $value     - Mandatory parameter:                   ???PARAMETER???
 * @param integer $width     - Optional parameter (default = 0):      ???PARAMETER???
 * @param boolean $mand      - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $dis       - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $nva       - Optional parameter (default = false):  ???PARAMETER???
 * @param string  $tooltip   - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $jsfunc    - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $form      - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $style_add - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $class_add - Optional parameter (default = ''):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fieldDate($fieldname, $value, $width=0, $mand=false, $dis=false, $nva=false, $tooltip='', $jsfunc='', $form='', $style_add='', $class_add='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(isset($_POST['mt_hl']) && $_POST['mt_hl'])
		$value=fxNow();

	$value=get_entry($value, 'datum');
	$tmp=fieldText($fieldname, $value, $width, 10, 'center', $mand, $dis, $nva, $tooltip, $jsfunc, 'date', $form, $style_add, $class_add);

	if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE) || ($GLOBALS['cpo'.$GLOBALS['fxpglobals']['tr']]))
	{
		$tmp = '<div style="width:auto;display:inline-block;white-space:nowrap;">'.$tmp;
		if($dis)
			$tmp .= '<img id="img_'.$fieldname.'" class="ainactive" src="'.$GLOBALS['gfxpath'].'im_calendar.png" style="vertical-align:top;margin:0 2px;">';
		else
			$tmp .= '<img id="img_'.$fieldname.'" class="date_pop fxfil" src="'.$GLOBALS['gfxpath'].'im_calendar.png" tooltip="'.meldung(1470, false).'" style="vertical-align:top;margin:0 2px;">';
		$tmp .= '</div>';
	}

//echo('<pre>'.fxHtmlEncode($tmp).'</pre>');
	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various $fieldname - Mandatory parameter:                   ???PARAMETER???
 * @param integer $width     - Optional parameter (default = 0):      ???PARAMETER???
 * @param boolean $multi     - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $mand      - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $dis       - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $nva       - Optional parameter (default = false):  ???PARAMETER???
 * @param string  $tooltip   - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $jsfunc    - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $form      - Optional parameter (default = ''):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fieldFile($fieldname, $width=0, $multi=false, $mand=false, $dis=false, $nva=false, $tooltip='', $jsfunc='', $form='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$h1=20;
	$h2=20;
	$wd=68;

	$csi=session_id();
	if(!strlen($csi))
	{
		session_id($GLOBALS['fxpglobals']['sesstoid']);
		@session_start();
	}

	$type='file';
	if($multi)
		$type .= 'm';

	$tmp  = '<span style="position:relative;">';
	$tmp .= '<span id="'.$fieldname.'_upload_status" class="fstatus" style="float:left;position:relative;left:0;width:50px;height:'.$h1.'px;padding:5px 4px;overflow:hidden;float:left;">Upload</span>';
	$tmp .= '<span style="position:relative;float:left;left:2px;">';
	$tmp .= '<form id="'.$fieldname.'_upload_form" name="'.$fieldname.'_upload_form" method="POST" enctype="multipart/form-data" action="loader.php?url=upload.inc&locstoid='.$GLOBALS['fxpglobals']['locstoid'].'&sesstoid='.$GLOBALS['fxpglobals']['sesstoid'].'&lts='.$GLOBALS['lts'].'&lcnt='.$GLOBALS['lcnt'].'" target="uploader_iframe">';
	if($GLOBALS['fxpglobals']['tr'] == 217)	// Formular-Design
		$tmp .= '<input type="hidden" id="upl_ktyp" name="upl_ktyp" value="0" /><input type="hidden" id="upl_mordner" name="upl_mordner" value="0" />';
	$tmp .= '<input type="hidden" id="frm" name="frm" value="'.$GLOBALS['_form'].'" /><input type="hidden" id="lts" name="lts" value="'.$GLOBALS['lts'].'" />';
	$tmp .= '<input type="hidden" id="upname" name="'.ini_get('session.upload_progress.name').'" value="'.$fieldname.'" />';
	$tmp .= fieldText($fieldname, '', $width-$wd, 0, 'left', $mand, $dis, $nva, $tooltip, fxf_jsFunction('uploadFile', '*onchange', $fieldname), $type, $form, 'height:'.$h2.'px;');
	$tmp .= '</form>';
	$tmp .= '</span>';
	$tmp .= '</div>';
//echo('<pre>'.fxHtmlEncode($tmp).'</pre>');

	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various $fieldname - Mandatory parameter:                ???PARAMETER???
 * @param string  $value     - Optional parameter (default = ''):  ???PARAMETER???
 * @param integer $width     - Optional parameter (default = 0):   ???PARAMETER???
 * @param integer $height    - Optional parameter (default = 0):   ???PARAMETER???
 * @param integer $multi     - Optional parameter (default = 0):   ???PARAMETER???
 * @param string  $tooltip   - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $jsfunc    - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $form      - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fieldFileDoc($fieldname, $value='', $width=0, $height=0, $multi=0, $tooltip='', $jsfunc='', $form='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$findex='';
	$idp=strpos($fieldname,'[');
	if($idp)
	{
		$findex=substr($fieldname,$idp);
		$fieldname=substr($fieldname,0,$idp);
	}

	if(strlen($form))
		setForm($form);

	// Filepath and -name
//fxDebug($value,'$value');
	$did=0;
	$rid=0;
	$fex=0;
	$fpt='';
	$opt='';
	$opn='';
	$fpn='';
	if(is_array($value))
	{
		if(isset($value['doc_dbid']) && !isset($value['new_name']))
			$did=(int)$value['doc_dbid'];
		if(isset($value['doc_drid']) && !isset($value['new_name']))
			$rid=(int)$value['doc_drid'];

		if(isset($value['new_name']))
			$fpt=$value['new_name'];
		if(isset($value['doc_path']))
		{
			$opt=$value['doc_path'];
			if(!strlen($fpt))
				$fpt=$opt;
		}

		if(isset($value['name']))
			$fpn=$value['name'];
		if(isset($value['doc_name']))
		{
			$opn=$value['doc_name'];
			if(!strlen($fpn))
				$fpn=$opn;
		}
	}
	else if(strlen($value))
		$fpt=$value;
	if(!strlen($fpn) && strlen($fpt))
	{
		$fpc=str_replace('\\','/',$fpt);
		$fpp=strrpos($fpc,'/');
		if($fpp !== false)
			$fpn=substr($fpt,$fpp+1);
		else
			$fpn=$fpt;
	}
	if(!strlen($opn) && strlen($opt))
	{
		$fpc=str_replace('\\','/',$opt);
		$fpp=strrpos($fpc,'/');
		if($fpp !== false)
			$opn=substr($opt,$fpp+1);
		else
			$opn=$opt;
	}
	if(!strlen($fpn))
	{
		if(isset($GLOBALS[$fieldname.'_fpn'.$findex]))
			$fpn='<i>'.$GLOBALS[$fieldname.'_fpn'.$findex].'</i>';
		if(!strlen($fpn))
		{
			$a=explode('|',meldung(3264,false));	// Kein Bild vorhanden!|Kein Dokument vorhanden!|Keine Datei vorhanden!
			$fpn='<i>'.$a[1].'</i>';				// -> Kein Dokument vorhanden!
		}
	}
	else
	{
		// File icon from suffix
		$sdp=strrpos($fpn, '.');
		if($sdp !== false)
		{
			$suffix=strtolower(substr($fpn,$sdp+1));
			if(file_exists($GLOBALS['gfxpath'].'ico_'.$suffix.'.png'))
				$fpn='<img src="'.$GLOBALS['gfxpath'].'ico_'.$suffix.'.png" align="top" />&nbsp;'.$fpn;
		}
		if(strlen($opn))
		{
			$sdp=strrpos($opn, '.');
			if($sdp !== false)
			{
				$suffix=strtolower(substr($opn,$sdp+1));
				if(file_exists($GLOBALS['gfxpath'].'ico_'.$suffix.'.png'))
					$opn='<img src="'.$GLOBALS['gfxpath'].'ico_'.$suffix.'.png" align="top" />&nbsp;'.$opn;
			}
		}
	}
	if(strlen($fpt) && file_exists($fpt))
	{
		if(substr($fpt,-4) == '.tmp')
			$fex=-1;
		else
			$fex=1;
	}

	// $width (= spaltenanzahl)
	$width=(int)$width;
	$dwidth=$width;
	$fwidth=$width;
	if($width > 0)
	{
		$dwidth='width:'.$width.'px;';
		$fwidth='width:'.max(0,$width-72).'px;';
	}
	else
	{
		$dwidth='';
		$fwidth='';
	}
	// $height (= zeilenanzahl)
	$height=(int)$height;
	$fheight=$height;
	if($height > 0)
		$fheight='height:'.$height.'px;';
	else
		$fheight='';
	// $multi (= anzeigegroesse1)
	$multi=(int)$multi;
	if($multi)
		$multi=1;
	// Deleteable
	if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
		$delable=true;
	else
		$delable=false;
	if(isset($GLOBALS[$fieldname.'_delable'.$findex]))
		$delable=$GLOBALS[$fieldname.'_delable'.$findex];
	// Uploadable
	if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
		$uplable=true;
	else
		$uplable=false;
	if(isset($GLOBALS[$fieldname.'_uplable'.$findex]))
		$uplable=$GLOBALS[$fieldname.'_uplable'.$findex];
	// Datei-Uploadtext
	$htext='';
	if(isset($GLOBALS[$fieldname.'_htext'.$findex]))
		$htext=$GLOBALS[$fieldname.'_htext'.$findex];
	// JavaScript
	if(!strlen($jsfunc) && $uplable)
		$jsfunc=fxf_jsFunction('uploader', 1, $multi, $fieldname, $htext).' ';
	$fjsfunc='';
	$ftooltip='';
	if($did && $fex)
	{
		// Rechte auf Dokument prüfen
		$right=getDocRights('id',$did);
		if($right)
		{
			if(!fxIsArray($GLOBALS['ddt']))
				$GLOBALS['ddt']=explode('|',meldung(3103));	// Display document in a new window|Document doesn't exist anymore|You don't have the necessary access rights for this document|This document type cannot be displayed|No document assigned yet!|This document has not been created yet!
			if(!isset($GLOBALS['tt_path']))
				$GLOBALS['tt_path']=meldung(1160,false);	// Path
			if(!isset($GLOBALS['tt_size']))
				$GLOBALS['tt_size']=get_text(1094,'10');	// Size

			if(!$GLOBALS['doc'])
				$GLOBALS['doc']=new fxpDoc();
			$adt=$GLOBALS['doc']->getDocType($fpn);
			$aca=getMIMETypes($adt,true);

			// ...Displayable?
			$ata='';
			if(!$aca['display'])
			{
				if(!isset($GLOBALS['ddtd']))
					$GLOBALS['ddtd']=meldung(3283,false);		// <br /><br /><i class='s2 lightgrey'>If the browser cannot display the document directly it will be offered<br />to download and/or to display with a different program</i>
				$ata=$GLOBALS['ddtd'];
			}
			$apt=fxReplaceSpecialCharacters($value['doc_dir'],false);
			$afn=fxReplaceSpecialCharacters($value['doc_file'],false);
			$fjsfunc=' '.fxf_jsFunction('dmsViewer', 'dsw', 'spath='.$apt.'&seid='.$did.'&sname='.$afn);
			$ftooltip='<b>'.$GLOBALS['ddt'][0].'</b><br /><br /><table><tr><td class=grey>'.$GLOBALS['tt_path'].':&nbsp;</td><td>'.$value['doc_dir'].'</td></tr><tr><td class=grey>'.$GLOBALS['tt_size'].':&nbsp;</td><td>'.convBytes($value['doc_size'], 'KB').'</td></tr></table>'.$ata;	// Display document in a new window
		}
	}
	// Tooltip
	$tt=$tooltip;
	if(!strlen($tt) && $uplable)
	{
		if(isset($GLOBALS[$fieldname.'_utt'.$findex]))
			$tt=$GLOBALS[$fieldname.'_utt'.$findex];
		else
		{
			$ta=explode('|', meldung(3262,false));	// Hier klicken zum Uploaden eines neuen Dokuments|Hier klicken zum Uploaden eines oder mehrerer neuen Dokumente
			if($multi)
				$tt=$ta[1];
			else
				$tt=$ta[0];
		}
	}
	if(strlen($tt))
		$tooltip='tooltip="'.$tt.'" ';
	$ftt=$ftooltip;
	if(strlen($ftt))
		$ftooltip='tooltip="'.$ftt.'" ';
	// Style
	$dstadd='';
	$ustadd='';
	if(strlen($jsfunc))
		$ustadd .= 'cursor:pointer;';
	else if(strlen($tooltip))
		$ustadd .= 'cursor:help;';
	$fstadd='';
	if(strlen($fjsfunc))
		$fstadd .= 'cursor:pointer;';
	else if($fex < 0)
		$fstadd .= 'opacity:0.44;';
	else if(!$fex)
		$fstadd .= 'opacity:0.22;';

	$tmp  = '<div id="'.$fieldname.'" name="'.$fieldname.'" fxform="'.$GLOBALS['_form'].'" utype="doc" style="position:relative;'.$dwidth.';padding:4px 0;background:#ffffff;border:1px solid #d2d2d2;border-right-color:#888888;border-bottom-color:#888888;border-top-right-radius:4px;border-bottom-right-radius:4px;box-shadow:3px 3px 3px rgba(0,0,0, 0.15);'.$dstadd.'">';
	$tmp .= '	<div style="'.$fwidth.$fheight.'padding:0 4px;overflow-x:hidden;overflow-y:auto;">';
	$tmp .= '		<div id="div_'.$fieldname.'" name="div_'.$fieldname.'" style="position:relative;">';
	if(strlen($opn) && ($opn != $fpn))
		$tmp .= '			<span id="span_'.$fieldname.'" name="span_'.$fieldname.'" style="opacity:0.22;">'.$opn.'</span>&nbsp;&nbsp;<span class="lightergrey">&rarr;</span>&nbsp;&nbsp;'.$fpn;
	else
		$tmp .= '			<span id="span_'.$fieldname.'" name="span_'.$fieldname.'" '.$ftooltip.$fjsfunc.'style="'.$fstadd.'">'.$fpn.'</span>';
	if(($fex > 0) && $delable)
		$tmp .= '			<img id="del_'.$fieldname.'"  name="del_'.$fieldname.'" src="'.$GLOBALS['gfxpath'].'cls_c_18x18.png" tooltip="'.meldung(2042,false).'" style="position:absolute;right:-2px;top:-1px;cursor:pointer;" '.fxf_jsFunction('deletePic', true, '').' />';	// Bild löschen
	$tmp .= '		</div>';
	$tmp .= '	</div>';
	$uih=22;
	$tmp .= '	<div '.$tooltip.$jsfunc.'style="position:absolute;right:0;top:0;bottom:0;width:64px;background:#dedede;border-top-right-radius:4px;border-bottom-right-radius:4px;text-align:center;'.$ustadd.'"><img src="'.$GLOBALS['gfxpath'].'upl_b_80x80.png" style="height:'.$uih.'px;" /></div>';
	$tmp .= '</div>';
//echo('<pre>'.fxHtmlEncode($tmp).'</pre>');

	fxSession('t','p',$fieldname,$value);

	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various $fieldname - Mandatory parameter:                        ???PARAMETER???
 * @param string  $value     - Optional parameter (default = ''):          ???PARAMETER???
 * @param string  $position  - Optional parameter (default = 'realtive'):  ???PARAMETER???
 * @param string  $align     - Optional parameter (default = ''):          ???PARAMETER???
 * @param integer $left      - Optional parameter (default = 0):           ???PARAMETER???
 * @param integer $top       - Optional parameter (default = 0):           ???PARAMETER???
 * @param integer $width     - Optional parameter (default = 0):           ???PARAMETER???
 * @param integer $height    - Optional parameter (default = 0):           ???PARAMETER???
 * @param string  $tooltip   - Optional parameter (default = ''):          ???PARAMETER???
 * @param string  $jsfunc    - Optional parameter (default = ''):          ???PARAMETER???
 * @param string  $form      - Optional parameter (default = ''):          ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fieldFilePic($fieldname, $value='', $position='realtive', $align='', $left=0, $top=0, $width=0, $height=0, $tooltip='', $jsfunc='', $form='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$findex='';
	$idp=strpos($fieldname,'[');
	if($idp)
	{
		$findex=substr($fieldname,$idp);
		$fieldname=substr($fieldname,0,$idp);
	}

	if(strlen($form))
		setForm($form);

	$multi=0;

	// Source
//fxDebug($value,'$value');
	$pex=false;
	$def=true;
	$src='';
	$mtime='';
	if(fxIsArray($value))
	{
		if(isset($value['new_name']))
			$src=$value['new_name'];
		else if(isset($value['dsp']))
			$src=$value['dsp'];

		if(isset($value['def']))
			$def=$value['def'];

		if(isset($value['mtime']))
			$mtime=$value['mtime'];
	}
	else if(strlen($value))
		$src=$value;

	if(!strlen($src) || !file_exists($src))
		$src=$GLOBALS['gfxpath'].'upl_c_80x80.png';
	if(!$def && file_exists($src) && (substr($src,-4) != '.tmp'))
		$pex=true;

	if(!strlen($mtime) && file_exists($src))
		$mtime='?'.filemtime($src);

	// $position (= vertausr):	top, middle, bottom, baseline
	if(strlen($position))
	{
		if($GLOBALS['check_baseline'] && ($position == 'baseline'))
			$position='absolute';
		else
			$position='relative';
		$position='position:'.$position.';';
	}
	// $align (= horausr):		left, center, right, justify, char
	if(strlen($align))
	{
		if(($align == 'justify') || ($align == 'char'))
			$align='';
		else
			$align='align="'.$align.'" ';
	}
	// $left (= spaltenanzahl)
	$left=(int)$left;
	if($left > 0)
		$left='left:'.$left.'px;';
	else if($left < 0)
		$left='right:'.abs($left).'px;';
	else
		$left='';
	// $top (= zeilenanzahl)
	$top=(int)$top;
	if($top > 0)
		$top='top:'.$top.'px;';
	else if($top < 0)
		$top='bottom:'.abs($top).'px;';
	else
		$top='';
	// $width (= anzeigegroesse1)
	$width=abs((int)$width);
	$pwidth=$width;
	if($width)
		$dwidth='width:'.$pwidth.'px;';
	else
		$dwidth='';
	// $height (= anzeigegroesse2)
	$height=abs((int)$height);
	$pheight=$height;
	if($height)
		$dheight='height:'.$pheight.'px;';
	else
		$dheight='';
	// Deleteable
	if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
		$delable=true;
	else
		$delable=false;
	if(isset($GLOBALS[$fieldname.'_delable'.$findex]))
		$delable=$GLOBALS[$fieldname.'_delable'.$findex];
	// Uploadable
	if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
		$uplable=true;
	else
		$uplable=false;
	if(isset($GLOBALS[$fieldname.'_uplable'.$findex]))
		$uplable=$GLOBALS[$fieldname.'_uplable'.$findex];
	// Tooltip
	$tt=$tooltip;
	if(!strlen($tt) && $uplable)
	{
		$ta=explode('|', meldung(3261,false));	// Hier klicken zum Uploaden eines neuen Bildes|Hier klicken zum Uploaden eines oder mehrerer neuen Bilder
		if($multi)
			$tt=$ta[1];
		else
			$tt=$ta[0];
	}
	if(strlen($tt))
		$tooltip='tooltip="'.$tt.'" ';
	// Bild-Uploadtext
	$htext='';
	if(isset($GLOBALS[$fieldname.'_htext'.$findex]))
		$htext=$GLOBALS[$fieldname.'_htext'.$findex];
	// JavaScript
	if(!strlen($jsfunc) && $uplable)
		$jsfunc=fxf_jsFunction('uploader', 1, $multi, $fieldname, $htext).' ';
	// Style
	$stadd='';
	if(!$pex)
		$stadd .= 'opacity:0.10;';
	if(strlen($jsfunc))
		$stadd .= 'cursor:pointer;';
	else if(strlen($tooltip))
		$stadd .= 'cursor:help;';
	// Picture width/height
	if(file_exists($src))
	{
		$ims=GetImageSize($src);
		if(fxIsArray($ims))
		{
			$mxw=$pwidth;
			$mxh=$pheight;
			$iwo=$ims[0];
			$iho=$ims[1];
			$iwn=$iwo;
			$ihn=$iho;
			if($mxw && ($iwo > $iho) && ($iwo != $mxw))
			{
				$iwn=$mxw;
				$ihn=floor($iwn*$iho / $iwo);
			}
			if($mxh && ($iho >= $iwo) && ($ihn != $mxh))
			{
				$iwn=floor($iwn*$mxh / $ihn);
				$ihn=$mxh;
			}
			$pwidth=$iwn;
			$pheight=$ihn;
		}
	}
	$ptop='';
	if($pheight < $height)
		$ptop='top:'.floor(($height-$pheight)/2).'px;';
	$tmp  = '<div id="div_'.$fieldname.'" name="div_'.$fieldname.'" '.$tooltip.$jsfunc.'style="'.$position.$left.$top.$dwidth.$dheight.'text-align:center;border:1px solid white;border-right-color:black;border-bottom-color:black;border-radius:8px;box-shadow:3px 3px 6px rgba(0,0,0, 0.5);'.$stadd.'">';
	$tmp .= '<img id="'.$fieldname.'" name="'.$fieldname.'" src="'.$src.$mtime.'" fxform="'.$GLOBALS['_form'].'" utype="pic" style="position:relative;'.$ptop.'width:'.$pwidth.'px;height:'.$pheight.'px;border-radius:8px;" />';
	if($pex && $delable)
		$tmp .= '<img id="del_'.$fieldname.'"  name="del_'.$fieldname.'" src="'.$GLOBALS['gfxpath'].'del_c_32x32.png" tooltip="'.meldung(2042,false).'" style="position:absolute;right:-14px;top:-14px;background:#ffffff;border-radius:16px;box-shadow:3px 3px 6px rgba(0,0,0, 0.5);cursor:pointer;" '.fxf_jsFunction('deletePic', true, '').' />';	// Bild löschen
	$tmp .= '</div>';
//echo('<pre>'.fxHtmlEncode($tmp).'</pre>');

	fxSession('t','p',$fieldname,$value);

	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various $fieldname - Mandatory parameter:                   ???PARAMETER???
 * @param various $value     - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $neg       - Optional parameter (default = false):  ???PARAMETER???
 * @param integer $width     - Optional parameter (default = 0):      ???PARAMETER???
 * @param string  $tooltip   - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $jsfunc    - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $form      - Optional parameter (default = ''):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fieldRead($fieldname, $value, $neg=false, $width=0, $tooltip='', $jsfunc='', $form='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$style='';

	if(isset($GLOBALS[$fieldname.'_style'.$findex]))
		$style .= $GLOBALS[$fieldname.'_style'.$findex];

	$style .= 'overflow:auto;';

	if($width)
	{
		$width=(int)($width * $GLOBALS['msk_wfac']);
		$style .= 'width:'.$width.'px;';
	}

	if(isset($GLOBALS[$fieldname.'_height'.$findex]))
		$style .= 'height:'.(int)$GLOBALS[$fieldname.'_height'.$findex].'px;';

	if(isset($_POST['mt_hl']) && $_POST['mt_hl'])
		$style .= 'box-shadow:0 0 4px #008800 inset;';

	$tmp=fieldHidden($fieldname,$value,'',$form);
	$tmp .= '<div id="ro_'.$fieldname.'" class="ro" tabindex="-1"';
	if(strlen($tooltip))
	{
		$style .= 'cursor:help;';
		$tmp .= ' tooltip="'.$tooltip.'"';
	}
	if(strlen($style))
		$tmp .= ' style="'.$style.'"';
	if(strlen($jsfunc))
		$tmp .= ' '.$jsfunc;
	$tmp .= '>';

	if($value === '')
		$tmp .= '&nbsp;';
	else if($neg)
	{
		if($GLOBALS['negcol'])
			$col='color="'.$GLOBALS['negcol'].'"';
		else
			$col='class="red"';
		$tmp .= '<font '.$col.'>'.$value.'</font>';
		$GLOBALS['negnumber']=true;
	}
	else
		$tmp .= $value;

	$tmp .= '</div>';
//echo('<pre>'.fxHtmlEncode($tmp).'</pre>');

	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various $fieldname - Mandatory parameter:                   ???PARAMETER???
 * @param various $value     - Mandatory parameter:                   ???PARAMETER???
 * @param integer $width     - Optional parameter (default = 0):      ???PARAMETER???
 * @param integer $height    - Optional parameter (default = 0):      ???PARAMETER???
 * @param integer $len       - Optional parameter (default = 0):      ???PARAMETER???
 * @param boolean $mand      - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $dis       - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $nva       - Optional parameter (default = false):  ???PARAMETER???
 * @param string  $tooltip   - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $jsfunc    - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $form      - Optional parameter (default = ''):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fieldTextarea($fieldname, $value, $width=0, $height=0, $len=0, $mand=false, $dis=false, $nva=false, $tooltip='', $jsfunc='', $form='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(strlen($form))
		setForm($form);

	$findex='';
	$idp=strpos($fieldname, '[');
	if($idp)
	{
		$findex=substr($fieldname,$idp);
		$fieldname=substr($fieldname,0,$idp);
	}

	$fvalues=mskField($fieldname);
	$errcode=errorCode($fieldname,$GLOBALS['_masknumber'],$findex);

	$style=$errcode['style'];

	if(isset($GLOBALS[$fieldname.'_style'.$findex]))
		$style .= $GLOBALS[$fieldname.'_style'.$findex];

	$tmp='';
	if($dis && ($dis !== 'yes'))
		$tmp .= fieldHidden($fieldname.$findex,$value);
	$tmp .= '<textarea class="fxfta';
	if($mand)
		$tmp .= 'm';
	$tabindex=0;
	if($nva || $dis)
	{
		$tmp .= 'g';
		$tabindex=-1;
	}
	$tmp .= '" id="'.$fieldname.$findex.'" name="'.$fieldname.$findex.'" tabindex="'.$tabindex.'" fxform="'.$GLOBALS['_form'].'" ref="fx_ta_'.$GLOBALS['_masknumber'].'"';
	if(isset($GLOBALS[$fieldname.'_len'.$findex]))
		$len=(int)$GLOBALS[$fieldname.'_len'.$findex];
	else if(isset($GLOBALS[$fieldname.'_len']))
		$len=(int)$GLOBALS[$fieldname.'_len'];
	if($len)
		$tmp .= ' maxlength="'.$len.'"';
	if($nva || $dis)
		$tmp .= ' disabled';

	// Width
	if($width)
	{
		$width=abs((int)($width * $GLOBALS['msk_wfac']));
		$style .= 'width:'.$width.'px;';
	}
	else
		$style .= 'width:100%;';
	// Height
	if($height)
	{
		$height=abs((int)$height);
		$style .= 'height:'.$height.'px;';
	}
	else
		$style .= 'height:42px;';
	// Tooltip
	if($errcode['id'])
		$tmp .= maskErrorTooltip($errcode['id'], $errcode['mnr'], $errcode['nr'], $errcode['txt']);
	else if(strlen($tooltip))
	{
		$style .= 'cursor:help;';
		$tmp .= ' tooltip="'.$tooltip.'"';
	}
	// Style
	if(strlen($style))
		$tmp .= ' style="'.$style.'"';
	if(strlen($jsfunc))
		$tmp .= ' '.$jsfunc;
	$tmp .= '>'.$value.'</textarea>';
//echo('<pre>'.fxHtmlEncode($tmp).'</pre>');

	// Save field info
	if(!$dis && (substr($fieldname,0,5) != 'temp_'))
		fxSession('t','p',$fieldname.$findex,$value);

	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various $fieldname - Mandatory parameter:                   ???PARAMETER???
 * @param various $value     - Mandatory parameter:                   ???PARAMETER???
 * @param various $checked   - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $mand      - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $dis       - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $nva       - Optional parameter (default = false):  ???PARAMETER???
 * @param string  $tooltip   - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $jsfunc    - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $form      - Optional parameter (default = ''):     ???PARAMETER???
 * @param boolean $wbr       - Optional parameter (default = false):  ???PARAMETER???
 * @param string  $style_add - Optional parameter (default = ''):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fieldCheckbox($fieldname, $value, $checked, $mand=false, $dis=false, $nva=false, $tooltip='', $jsfunc='', $form='', $wbr=false, $style_add='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(strlen($form))
		setForm($form);

	$value=trim($value);
	if(!strlen($value))
		$value='1';

	if(strlen($checked))
		$checked=trim(strtolower(strip_tags($checked)));
	if(strlen($checked) && ($checked != '0') && ($checked != 'nein') && ($checked != 'no'))
		$checked=1;
	else
		$checked=0;

	$findex='';
	$idp=strpos($fieldname, '[');
	if($idp)
	{
		$findex=substr($fieldname,$idp);
		$fieldname=substr($fieldname,0,$idp);
	}

	$errcode=errorCode($fieldname,$GLOBALS['_masknumber'],$findex);

	$style=$errcode['style'];

	if(isset($GLOBALS[$fieldname.'_style'.$findex]))
		$style .= $GLOBALS[$fieldname.'_style'.$findex];
	if(strlen($style_add))
		$style .= $style_add;

	if(!$dis && (isset($GLOBALS[$fieldname.'_dis']) || isset($GLOBALS[$fieldname.'_dis'.$findex])))
		$dis=true;

	$tmp='';
	if($dis && ($dis !== 'yes'))
		$tmp .= fieldHidden($fieldname.$findex,$checked);
	$tmp .= '<input class="fxfcb';
	if($mand)
		$tmp .= 'm';
	$tabindex=0;
	if($nva || $dis)
	{
		$tmp .= 'g';
		$tabindex=-1;
	}
	$tmp .= '" id="'.$fieldname.$findex.'" name="'.$fieldname.$findex.'" type="checkbox" tabindex="'.$tabindex.'" fxform="'.$GLOBALS['_form'].'" ref="fx_cb_'.$GLOBALS['_masknumber'].'" value="'.$value.'"';
	if($nva || $dis)
		$tmp .= ' disabled';
	if($checked)
		$tmp .= ' checked';
	// Tooltip
	if(strlen($tooltip))
	{
		$style .= 'cursor:help;';
		$tmp .= ' tooltip="'.$tooltip.'"';
	}
	// Style
	if(strlen($style))
		$tmp .= ' style="'.$style.'"';
	if(strlen($jsfunc))
		$tmp .= ' '.$jsfunc;
	$tmp .= '>';
	if($wbr)
		$tmp .= '<br />';

	// Save field info
	if(!$dis)
	{
		if(substr($fieldname,0,5) != 'temp_')
		{
			if($checked)
				fxSession('t','p',$fieldname.$findex,$value);
			else
				fxSession('t','u',$fieldname.$findex,$value);
		}

		// Set global checkbox info
		$cfn=$fieldname;
		if(isset($GLOBALS[$cfn.'_alias']))
			$cfn=$GLOBALS[$cfn.'_alias'].'|'.$cfn;
		if(!fxIsArray($GLOBALS['fxptdata']['chkf_'.$GLOBALS['_masknumber']]))
			$GLOBALS['fxptdata']['chkf_'.$GLOBALS['_masknumber']]=array();
		if(!isset($GLOBALS['fxptdata']['chkf_'.$GLOBALS['_masknumber']][$cfn]))
			$GLOBALS['fxptdata']['chkf_'.$GLOBALS['_masknumber']][$cfn]=1;
		else
			$GLOBALS['fxptdata']['chkf_'.$GLOBALS['_masknumber']][$cfn]++;
	}
//echo('<pre>'.fxHtmlEncode($tmp).'</pre>');

	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various $fieldname     - Mandatory parameter:                     ???PARAMETER???
 * @param various $value         - Mandatory parameter:                     ???PARAMETER???
 * @param various $value_array   - Mandatory parameter:                     ???PARAMETER???
 * @param string  $direction     - Optional parameter (default = 'h'):      ???PARAMETER???
 * @param integer $maxcol        - Optional parameter (default = 0):        ???PARAMETER???
 * @param boolean $dis           - Optional parameter (default = false):    ???PARAMETER???
 * @param boolean $nva           - Optional parameter (default = false):    ???PARAMETER???
 * @param array   $tooltip_array - Optional parameter (default = array()):  ???PARAMETER???
 * @param string  $jsfunc        - Optional parameter (default = ''):       ???PARAMETER???
 * @param string  $form          - Optional parameter (default = ''):       ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fieldRadio($fieldname, $value, $value_array, $direction='h', $maxcol=0, $dis=false, $nva=false, $tooltip_array=array(), $jsfunc='', $form='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(strlen($form))
		setForm($form);

	$direction=strtolower(substr(trim($direction),0,1));	// h=horizontal (default, von links nach rechts) + v=vertikal (von oben nach unten)

	$findex='';
	$idp=strpos($fieldname, '[');
	if($idp)
	{
		$findex=substr($fieldname,$idp);
		$fieldname=substr($fieldname,0,$idp);
	}

	$errcode=errorCode($fieldname,$GLOBALS['_masknumber'],$findex);

	$style=$errcode['style'];

	if(isset($GLOBALS[$fieldname.'_style'.$findex]))
		$style .= $GLOBALS[$fieldname.'_style'.$findex];

	$tmp='';
	if($dis && ($dis !== 'yes'))
		$tmp .= fieldHidden($fieldname.$findex,$value);
	if(fxIsArray($value_array))
	{
		$rc=0;
		$it=false;
		if($maxcol && ($maxcol < sizeof($value_array)) && ($direction != 'v'))
			$it=true;
		if($it || ($direction == 'v'))
			$tmp .= '<table border="0" cellpadding="0" cellspacing="0">';
		$ltxt='';
		foreach($value_array as $val => $txt)
		{
			$rstyle=$style;

			if(($it && ($rc%$maxcol == 0)) || ($direction == 'v'))
				$tmp .= '<tr>';
			if($it || ($direction == 'v'))
				$tmp .= '<td valign="top" style="padding-right:8px;">';
			if($rc && !$it)
			{
				if(($direction != 'v') && strlen($ltxt))	// horizontal (= default)
					$tmp .= '&nbsp;&nbsp;&nbsp;';
			}
			$tmp .= '<input class="fxfra';
			if($mand)
				$tmp .= 'm';
			$tabindex=0;
			if($nva || $dis)
			{
				$tmp .= 'g';
				$tabindex=-1;
			}
			$tmp .= '" id="'.$fieldname.$findex.'" name="'.$fieldname.$findex.'" type="radio" tabindex="'.$tabindex.'" fxform="'.$GLOBALS['_form'].'" value="'.$val.'"';
			$rdis=false;
			if($nva || $dis || ($txt == '{disabled}'))
			{
				$rdis=true;
				$tmp .= ' disabled';
			}
			if($value == $val)
				$tmp .= ' checked';
			// Tooltip
			if(!$rdis && fxIsArray($tooltip_array) && isset($tooltip_array[$val]))
			{
				$rstyle .= 'cursor:help;';
				$tmp .= ' tooltip="'.$tooltip_array[$val].'"';
			}
			// Style
			if(strlen($rstyle))
				$tmp .= ' style="'.$rstyle.'"';
			if(strlen($jsfunc))
				$tmp .= ' '.$jsfunc;
			$tmp .= '>';
			if($direction == 'v')
				$tmp .= '</td><td>';
			if($txt == '{disabled}')
				$txt='';
			if(strlen($txt))
			{
				if($direction != 'v')
					$tmp .= '&nbsp;';
				$tmp .= $txt;
			}
			$ltxt=$txt;

			$rc++;

			if($it || ($direction == 'v'))
				$tmp .= '</td>';
			if(($it && ($rc%$maxcol == 0)) || ($direction == 'v'))
				$tmp .= '</tr>';
		}

		if($it && ($rc%$maxcol != 0))
		{
			$cs=0;
			while($rc%$maxcol != 0)
			{
				$cs++;
				$rc++;
			}
			$tmp .= '<td colspan="'.$cs.'"></td>';
			$tmp .= '</tr>';
		}
		if($it || ($direction == 'v'))
			$tmp .= '</table>';
	}

	// Save field info
	if(!$dis && (substr($fieldname,0,5) != 'temp_'))
		fxSession('t','p',$fieldname.$findex,$value);

	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various $fieldname   - Mandatory parameter:                   ???PARAMETER???
 * @param various $value       - Mandatory parameter:                   ???PARAMETER???
 * @param integer $width       - Optional parameter (default = 0):      ???PARAMETER???
 * @param integer $height      - Optional parameter (default = 0):      ???PARAMETER???
 * @param boolean $mand        - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $dis         - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $nva         - Optional parameter (default = false):  ???PARAMETER???
 * @param string  $tooltip     - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $jsfunc      - Optional parameter (default = ''):     ???PARAMETER???
 * @param boolean $changeinput - Optional parameter (default = false):  ???PARAMETER???
 * @param string  $form        - Optional parameter (default = ''):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fieldSelect($fieldname, $value, $width=0, $height=0, $mand=false, $dis=false, $nva=false, $tooltip='', $jsfunc='', $changeinput=false, $form='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(strlen($form))
		setForm($form);

	$findex='';
	$idp=strpos($fieldname, '[');
	if($idp)
	{
		$findex=substr($fieldname, $idp);
		$fieldname=substr($fieldname, 0, $idp);
	}

	$cfieldname=$fieldname;
	if(isset($GLOBALS[$fieldname.'_af']))
		$cfieldname=$GLOBALS[$fieldname.'_af'];

	$fvalues=mskField($cfieldname);
	$errcode=errorCode($fieldname, $GLOBALS['_masknumber'],$findex);

	$sel_f='auswahl_'.$cfieldname;
	$ef=function_exists($sel_f);
	if(isset($GLOBALS[$fieldname.'_ds']))
		$fnds=$GLOBALS[$fieldname.'_ds'];
	else
		$fnds='';
	if(isset($GLOBALS[$fieldname.'_aci']))
		$changeinput=$GLOBALS[$fieldname.'_aci'];

	$tooltip=trim($tooltip);
	if(strlen($tooltip))
	{
		$tt=$fieldname.'_tt';
		if(!isset($GLOBALS[$tt]))
			$GLOBALS[$tt]=$tooltip;
	}

	$jsfunc=trim($jsfunc);
	if(strlen($jsfunc))
	{
		$jsfunc_it=$fieldname.'_jf';
		if(!isset($GLOBALS[$jsfunc_it]))
			$GLOBALS[$jsfunc_it]=$jsfunc;
	}

	if(!$dis && isset($GLOBALS[$fieldname.'_dis']))
		$dis=true;

	$a_arg_array=array('a_name'=>$fieldname, 'a_select'=>$value, 'a_muss'=>$mand, 'a_dis'=>$dis, 'a_width'=>$width, 'a_height'=>$height, 'a_index'=>$findex, 'a_defsel'=>$fnds, 'a_change'=>$changeinput, 'a_tabs'=>1);
//fxDebug($a_arg_array, '$a_arg_array: '.$fieldname);
	// Selectfeld
	if(isset($GLOBALS[$cfieldname.'select']))
		$jsf=false;
	else if($ef)
		$jsf=true;
	else
		$jsf=false;

	// Funktion für Auswahlfeld existiert
	if($jsf)
	{
		$tmp=call_user_func($sel_f, 0, $a_arg_array, $fieldname);
	}
	else
	{
		// Selectfeld
		if(isset($GLOBALS[$fieldname.'select']))
			$arg_array=array('s_name'=>$GLOBALS[$fieldname.'select'], 's_func'=>$GLOBALS[$fieldname.'selectfunc'], 's_feld'=>$GLOBALS[$fieldname.'selectfeld'], 's_wert'=>$GLOBALS[$fieldname.'selectwert'], 'a_name'=>$fieldname, 'a_select'=>$value, 'a_change'=>$changeinput);
		// Nachschlagewert
		else
			$arg_array=array('s_id'=>$fvalues['id'], 'a_name'=>$fieldname, 'a_select'=>$value, 'a_change'=>$changeinput);
		if(isset($GLOBALS[$fieldname.'_array']) && fxIsArray($GLOBALS[$fieldname.'_array']))
			$a_array=$GLOBALS[$fieldname.'_array'];
		else if(isset($GLOBALS[$fieldname.'select']) && fxIsArray($GLOBALS[$fieldname.'select']))
			$a_array=$GLOBALS[$fieldname.'select'];
		else
			$a_array=select_array($arg_array);
//fxDebug($a_array, '$a_array: '.$fieldname);

		// Spezial: Anzeigewerte für Fördermodelle filtern
		if(($fieldname == 'fpp_fmodell') && fxIsArray($a_array))
		{
			foreach($a_array as $oid => $otxt)
			{
				$opp=strpos($otxt, '|');
				if($opp)
					$a_array[$oid]=substr($otxt, 0, $opp);
			}
		}

		$tmp=convert_array(2, $a_array, $a_arg_array);
	}
//echo('<pre>'.fxHtmlEncode($tmp).'</pre>');

	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various $fieldname - Mandatory parameter:                   ???PARAMETER???
 * @param various $value     - Mandatory parameter:                   ???PARAMETER???
 * @param string  $type      - Optional parameter (default = 'sub'):  ???PARAMETER???
 * @param integer $width     - Optional parameter (default = 0):      ???PARAMETER???
 * @param boolean $dis       - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $nva       - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $force     - Optional parameter (default = false):  ???PARAMETER???
 * @param string  $ltr       - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $gparam    - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $jsfunc    - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $style     - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $form      - Optional parameter (default = ''):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fieldSubmit($fieldname, $value, $type='sub', $width=0, $dis=false, $nva=false, $force=false, $ltr='', $gparam='', $jsfunc='', $style='', $form='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(strlen($form))
		setForm($form);

	$baseline=false;
	if($force === 0)
		$force=false;
	else if($force || (fxIsArray($value) && ($value['vertausr'] == 'baseline')))
		$baseline=true;

	$findex='';
	$idp=strpos($fieldname, '[');
	if($idp)
	{
		$findex=substr($fieldname, $idp);
		$fieldname=substr($fieldname, 0, $idp);
	}
//echo('<b class="red">fieldSubmit</b>: $fieldname='.$fieldname.'<br />');

	$btype='';
	if(substr($type,0,1) == 'b')
	{
		$btype='b';
		$type=substr($type,1);
	}
	$type=substr($type,0,3);
	if($type == 'res')
		$ftype='reset';
	else
		$ftype='submit';

	// Get button width and text
	$btnvalue='';
	$spezial='';
	$multi=false;
	if(fxIsArray($value))
	{
		$showlit=-1;
		if($value['anzeigegroesse1'])
		{
			$showlit=$value['anzeigegroesse1'];
			if($showlit > 6)
				$showlit=$showlit%6;
		}

		if($value['feldname'] == 'Button_Multi')
			$multi=true;

		if(strlen($value['literal_overwrite']))
		{
			if($value['literal_overwrite'] == 1580)	// Button_Multi
				$multi=true;
			else
				$btnvalue=$value['literal_value'];
		}
		else if($showlit == 5)
			$btnvalue=$value['text_kurz'];
		else if($showlit == 6)
			$btnvalue=$value['literal_text'];
		else if($showlit < 0)
		{
			$showlit=3;
			$sbv=strlen($btnvalue);
			if(!$sbv)
				$btnvalue=$value[$GLOBALS['anzart_defines'][$showlit]];
		}

		if(!strlen($btnvalue))
			$btnvalue=$value['literal_value'];

		if(strlen($value['spezial']))
			$spezial=strtolower(trim($value['spezial']));
	}
	else
		$btnvalue=$value;

	// Check, if global button text
	$nc=$fieldname.'_btxt';
	if(isset($GLOBALS[$nc]))
		$btnvalue=strip_tags($GLOBALS[$nc]);
	else if($multi)
	{
		if($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY)
			$btnvalue=get_text(358,'15');	// Display
		else if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
			$btnvalue=get_text(282,'15');	// Delete
		else
			$btnvalue=get_text(1360,'15');	// Save
	}

	// Check, if global button width or size
	$lc=$fieldname.'_bwidth';
	if(isset($GLOBALS[$lc]))
		$width=(int)$GLOBALS[$lc];
	else
		$lc=$fieldname.'_bsize';

	// Check, if button is pressed
	$bp=false;
	$bc=$fieldname.'_bp';
	if(isset($GLOBALS[$bc]) && $GLOBALS[$bc])
		$bp=true;

	// Button class
	$cls='fxfb'.substr($ftype,0,1).$btype;
	if($nva || ($dis && !strlen($findex)))
		$cls='dis';

	$tmp='<input id="'.$fieldname.$findex.'" name="'.$fieldname.$findex.'" class="'.$cls.'" type="'.$ftype.'" fxform="'.$GLOBALS['_form'].'" value="'.$btnvalue.'"';
	if(strlen($spezial))
		$tmp .= ' ref="'.$spezial.'"';
	if(strlen($ltr))
		$tmp .= ' tr="'.(int)$ltr.'"';
	if(strlen($gparam))
		$tmp .= ' get="'.trim($gparam).'"';
	if($GLOBALS['popuptr'])
		$tmp .= ' ptr="'.(int)$GLOBALS['popuptr'].'"';
	if($baseline && ($GLOBALS['fxpglobals']['tr'] != 175))
		$tmp .= ' baseline';
	else if($width>0)
		$style .= 'width:'.$width.'px;';
	$style=$style.'margin-left:8px;';
	if($bp)
		$style=$style.'border-color:#000;background:#009f6b;';
	$tmp .= ' style="'.$style.'"'.$jsfunc.'>'.$GLOBALS['nl'];

	$ftr=false;
	if(fxIsArray($_GET) && isset($_GET['pname']) && (substr($_GET['pname'],-4) == '.png'))
		$ftr=true;
//echo($fieldname.': <pre>'.fxHtmlEncode($tmp).'</pre>');

	// Button buffer for button bar
	if($force)
	{
		$GLOBALS['fxpvars']['bbuffer'][-1] .= $tmp;
		$tmp = '';
	}
	else if($baseline && !$ftr)
	{
		$GLOBALS['fxpvars']['bbuffer'][$GLOBALS['_masknumber']] .= $tmp;
		$tmp = '';
	}
//echo($fieldname.': <pre>'.fxHtmlEncode($tmp).'</pre>');

	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various $fieldname - Mandatory parameter:                   ???PARAMETER???
 * @param various $pic       - Mandatory parameter:                   ???PARAMETER???
 * @param string  $value     - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $tooltip   - Optional parameter (default = ''):     ???PARAMETER???
 * @param boolean $dis       - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $nva       - Optional parameter (default = false):  ???PARAMETER???
 * @param string  $ltr       - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $gparam    - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $jsfunc    - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $form      - Optional parameter (default = ''):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fieldSubmitImage($fieldname, $pic, $value='', $tooltip='', $dis=false, $nva=false, $ltr='', $gparam='', $jsfunc='', $form='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(strlen($form))
		setForm($form);

	$findex='';
	$idp=strpos($fieldname, '[');
	if($idp)
	{
		$findex=substr($fieldname, $idp);
		$fieldname=substr($fieldname, 0, $idp);
	}

	if(fxIsArray($pic))
	{
		// Falls der Bildername in 'html_grafik' mit einem $ beginnt, dann die entspr. Variable verwenden
		$picname=$pic['html_grafik'];
		if(substr($picname,0,1) == '$')
			$picname=$GLOBALS[substr($picname,1)];
		else if(substr($picname,0,1) == '§')
			$picname=$value;

		// Falls der title in 'literal_20' mit einem $ beginnt, dann die entspr. Variable verwenden
		if(!strlen($tooltip))
		{
			$tooltip=$pic['literal_20'];
			if(substr($tooltip,0,1) == '$')
				$tooltip=$GLOBALS[substr($pic['literal_20'],1)];
		}

		if(fxIsArray($picname))
			$picname=$picname[0];
		$picname=strtolower($picname);
	}
	else
		$picname=$pic;

	$src=$GLOBALS['gfxpath'].$picname.'.png';
	if(!file_exists($src))
		$src=$GLOBALS['gfxpath'].$picname.'.jpg';
	if(!file_exists($src))
		$src=$GLOBALS['gfxpath'].$picname.'.gif';
	if(!file_exists($src))
		$src=$GLOBALS['gfxpath'].$picname.'.png';
	if(!file_exists($src))
		$src=$GLOBALS['gfxpath'].$picname.'.jpg';
	if(!file_exists($src))
		$src=$GLOBALS['gfxpath'].$picname.'.gif';
	if(!file_exists($src))
		$src=$pic;
	$picname=$src;

	$tmp='<input id="'.$fieldname.$findex.'" name="'.$fieldname.$findex.'" class="fxfis';
	$tabindex=0;
	if($nva || $dis)
	{
		$tmp .= 'g';
		$tabindex=-1;
	}
	$tmp .= '" type="image" tabindex="'.$tabindex.'" fxform="'.$GLOBALS['_form'].'" border="0"';
	if($nva || $dis)
		$tmp .= ' disabled';
	$tmp .= ' src="'.$picname.'"';
	if(strlen($ltr))
		$tmp .= ' tr="'.(int)$ltr.'"';
	if(strlen($gparam))
		$tmp .= ' get="'.trim($gparam).'"';
	if($GLOBALS['popuptr'])
		$tmp .= ' ptr="'.(int)$GLOBALS['popuptr'].'"';
	if(strlen($tooltip) && !$nva && !$dis)
		$tmp .= ' tooltip="'.$tooltip.'"';
//	$tmp .= ' style="margin-left:8px;"';
	$tmp .= $jsfunc.'>';
//echo('<pre>'.fxHtmlEncode($tmp).'</pre>');

	return $tmp;
}

/**
 * Create the HTML code of an image link
 *
 * @param string  $fieldname - Mandatory parameter:                Name of the field element
 * @param string  $picname   - Mandatory parameter:                Filename of the image (normally without path or suffix)
 * @param string  $href      - Mandatory parameter:                Link
 * @param string  $tooltip   - Optional parameter (default = ''):  Tooltip text
 * @param string  $dtr       - Optional parameter (default = ''):  Destination program function number
 * @param string  $style     - Optional parameter (default = ''):  Additional CSS style information
 * @param string  $cls       - Optional parameter (default = ''):  Additional CSS class information
 * @param integer $width     - Optional parameter (default = 0):   Width of the image in pixel
 * @param integer $height    - Optional parameter (default = 0):   Height of the image in pixel
 *
 * @return HTML code of the image link
 */
function fieldLinkImage($fieldname, $picname, $href, $tooltip='',$dtr='', $style='', $cls='', $width=0, $height=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$findex='';
	$idp=strpos($fieldname, '[');
	if($idp)
	{
		$findex=substr($fieldname, $idp);
		$fieldname=substr($fieldname, 0, $idp);
	}

	$src=$GLOBALS['gfxpath'].$picname.'.png';
	if(!file_exists($src))
		$src=$GLOBALS['gfxpath'].$picname.'.jpg';
	if(!file_exists($src))
		$src=$GLOBALS['gfxpath'].$picname.'.gif';
	if(!file_exists($src))
		$src=$picname;

	$cls=trim($cls);
	$style=trim($style);
	if(strlen($href))
	{
		if(strlen($cls))
			$cls .= ' ';
		$cls .= 'fxfil';
	}
	else
	{
/*
		if(strlen($cls))
			$cls .= ' ';
		$cls .= 'op25';
*/
		if(strlen($tooltip) && (!strlen($style) || (strpos($style,'cursor') === false)))
			$style .= 'cursor:help;';
	}
	if(strlen($cls))
		$cls=' class="'.$cls.'"';
	$style='margin:0;padding:0;'.$style;
	if(strlen($style))
		$style=' style="'.$style.'"';

	if(strlen($dtr))
		$dtr=' tr="'.$dtr.'"';
	$tmp='<img id="'.$fieldname.$findex.'" name="'.$fieldname.$findex.'"'.$cls.' src="'.$src.'" tooltip="'.$tooltip.'"';
	if($width)
		$tmp .= ' width="'.$width.'"';
	if($height)
		$tmp .= ' height="'.$height.'"';
	if(strlen($href))
		$tmp .= ' href="'.$href.'"';
	$tmp .= $dtr.$style.'>';
//echo(__FILE__.' ('.__LINE__.') -- <b>fieldLinkImage:</b><br /><pre>'.fxHtmlEncode($tmp).'</pre><hr />'.$GLOBALS['nl']);

	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various $fieldname - Mandatory parameter:                ???PARAMETER???
 * @param various $content   - Mandatory parameter:                ???PARAMETER???
 * @param various $href      - Mandatory parameter:                ???PARAMETER???
 * @param string  $tooltip   - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $dtr       - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $style     - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fieldLink($fieldname,$content,$href,$tooltip='',$dtr='',$style='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$findex='';
	$idp=strpos($fieldname, '[');
	if($idp)
	{
		$findex=substr($fieldname, $idp);
		$fieldname=substr($fieldname, 0, $idp);
	}

	if(strlen($dtr))
		$dtr=' tr="'.$dtr.'"';

	$tmp='<span id="'.$fieldname.$findex.'" name="'.$fieldname.$findex.'" class="fxfil"';
	if(strlen($tooltip))
		$tmp .= ' tooltip="'.$tooltip.'"';
	$tmp .= ' href="'.$href.'"'.$dtr;
	$style .= 'margin-left:8px;';
	if(strlen($style))
		$tmp .= ' style="'.$style.'"';
	$tmp .= '>'.$content.'</span>';

	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various $fname  - Mandatory parameter: ???PARAMETER???
 * @param various $mnr    - Mandatory parameter: ???PARAMETER???
 * @param various $findex - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function errorCode($fname,$mnr,$findex)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($mnr))
		$mnr=0;

	if(strlen($findex))
	{
		$fia=explode(']',str_replace('[','',$findex));
		if((sizeof($fia) == 2) && !strlen($fia[1]))
			unset($fia[1]);
		if(sizeof($fia) > 1)
			$mna=trim($fia[1]);
		else
			$mna=trim($fia[0]);
		if(strlen($mna))
			$mnr .= '_'.$mna;
	}

	// Farbcode für Fehlermeldungen
	$ec=array('id'=>"", 'st'=>0, 'bcol'=>"", 'col'=>"", 'style'=>"", 'mnr'=>0, 'nr'=>0, 'txt'=>"");
//echo('errorCode: $fname='.$fname.', $mnr='.$mnr.', $findex='.$findex.'<br />');
	if(fxIsArray($GLOBALS['fxpvars']['lookups'][$mnr][$fname]))
	{
		$posnr=(int)$GLOBALS['fxpvars']['lookups'][$mnr][$fname]['position'];
		$cec=errorCol($posnr);
		foreach($cec as $ck => $cv)
			$ec[$ck]=$cv;
		$ec['nr']=$GLOBALS['fxpvars']['lookups'][$mnr][$fname]['id'];
		$ec['txt']=str_ireplace('<br />','#b#', $GLOBALS['fxpvars']['lookups'][$mnr][$fname]['tabwert']);
		$ec['txt']=strip_tags($ec['txt']);
		$ec['txt']=str_replace('#b#', '<br />', $ec['txt']);
	}

	return $ec;
}

/**
 * ???FUNCTION???
 *
 * @param various $psgin - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function errorCol($psgin)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$psg=strtolower(trim($psgin));
	$pin=(int)$psgin;

	// Farbcode für Fehlermeldungen
	$ec=array('id'=>"", 'st'=>0, 'bcol'=>"", 'col'=>"", 'style'=>"", 'mnr'=>0);
	if(($psg == 'en') || ($pin > 99999) && ($pin < 200000))			// green -> success
	{
		$ec['id']='en';
		$ec['st']=1;
		$ec['bcol']='008800';
		$ec['col']='00aa00';
		$ec['mnr']=1216;
	}
	else if(($psg == 'ew') || ($pin > 199999) && ($pin < 300000))	// yellow -> warning
	{
		$ec['id']='ew';
		$ec['st']=2;
		$ec['bcol']='cccc00';
		$ec['col']='aaaa00';
		$ec['mnr']=1217;
	}
	else if(($psg == 'e') || ($pin > 299999) && ($pin < 400000))	// orange -> error
	{
		$ec['id']='e';
		$ec['st']=3;
//		$ec['bcol']='dd8844';	// orange disabled, because of traffic light colors -> red
//		$ec['col']='ffaa66';
		$ec['bcol']='e1001a';
		$ec['col']='ff3344';
		$ec['mnr']=1218;
	}
	else if(($psg == 'ef') || ($pin > 399999))						// red -> fatal error
	{
		$ec['id']='ef';
		$ec['st']=3;
		$ec['bcol']='e1001a';
		$ec['col']='ff3344';
		$ec['mnr']=1219;
	}
	if(strlen($ec['col']) && strlen($ec['bcol']))
		$ec['style']='border-color:#'.$ec['bcol'].';';

	return $ec;
}

/**
 * ???FUNCTION???
 *
 * @param various $aktzeile - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $wn       - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function tro($aktzeile, $wn=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($GLOBALS['tropen'] != -666)
		$GLOBALS['tropen']++;
	$GLOBALS['trcounter']++;
	if(isset($GLOBALS['trinfo'.$GLOBALS['_maskcounter'].'_'.$aktzeile]))
		$h='<tr';
	else
		$h='<tr id="tro'.$GLOBALS['trcounter'].'"';
//	if($GLOBALS['tropen'] != -666)
//		$h .= ' tc="'.($GLOBALS['tropen']-1).':'.$GLOBALS['tropen'].'"';
	if($wn)
		$h .= '>'.$GLOBALS['nl'];
	return $h;
}

/**
 * ???FUNCTION???
 *
 * @param boolean $wn - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function trc($wn=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($GLOBALS['tropen'] != -666)
		$GLOBALS['tropen']--;
	$h='</tr';
//	if($GLOBALS['tropen'] != -666)
//		$h .= ' tc="'.($GLOBALS['tropen']+1).':'.$GLOBALS['tropen'].'"';
	$h .= '>';
	if($wn)
		$h .= $GLOBALS['nl'];
	return $h;
}

/**
 * ???FUNCTION???
 *
 * @return ???RETURN???
 */
function tdo()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($GLOBALS['tdopen'] != -666)
		$GLOBALS['tdopen']++;
	$h='<td';
//	if($GLOBALS['tdopen'] != -666)
//		$h .= ' tc="'.($GLOBALS['tdopen']-1).':'.$GLOBALS['tdopen'].'"';
	return $h;
}

/**
 * ???FUNCTION???
 *
 * @param boolean $wn - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function tdc($wn=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($GLOBALS['tdopen'] != -666)
		$GLOBALS['tdopen']--;
	$h='</td';
//	if($GLOBALS['tdopen'] != -666)
//		$h .= ' tc="'.($GLOBALS['tdopen']+1).':'.$GLOBALS['tdopen'].'"';
	$h .= '>';
	if($wn)
		$h .= $GLOBALS['nl'];
	return $h;
}

/**
 * ???FUNCTION???
 *
 * @param string $form     - Optional parameter (default = ''): ???PARAMETER???
 * @param string $listline - Optional parameter (default = ''): ???PARAMETER???
 */
function setForm($form='',$listline='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//$trace=debug_backtrace();
//echo($trace[0]['file'].' ['.$trace[0]['line'].']: setForm ('.$form.','.$listline.')<br />');

	if(is_bool($form))
	{
		if($form)
		{
			if(strlen($listline))
				$form = 'lstmask'.$listline;
			else
				$form = 'datmask';
		}
		else
			$form = 'selmask';
		$GLOBALS['_form']='_fxform_'.$form;
	}
	else if(strlen($form))
		$GLOBALS['_form']='_fxform_'.$form;
	else if(!strlen($GLOBALS['_form']) || (substr($GLOBALS['_form'],11,4) == 'mask'))
	{
		$selmask=false;
		if(fxIsArray($GLOBALS['fx_mask']) && fxIsArray($GLOBALS['fx_mask'][$GLOBALS['_masknumber']]) && $GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['auswahlmaske'])
			$selmask=true;
		if($selmask)
			$GLOBALS['_form']='_fxform_selmask';
		else if(strlen($listline))
			$GLOBALS['_form']='_fxform_lstmask'.$listline;
		else
			$GLOBALS['_form']='_fxform_datmask';
	}

	if(substr($GLOBALS['_form'],-2,1) != '_')
		$GLOBALS['_form'] .= '_'.$GLOBALS['fxpglobals']['action'];
//echo('_form='.$GLOBALS['_form'].'<br />');
}

/**
 * ???FUNCTION???
 *
 * @param various $mn - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dummyMask($mn)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$dfa=array();
	$GLOBALS['fx_mfields'][$mn]=array();

	// Check if user is allowed to see cost fields
	$btype=strtoupper(substr($GLOBALS['fxpglobals']['settings']['budgetart'], 0, 1));	// 'N'one, 'I'ntern, 'E'xtern or 'B'oth
	$cst=darf_kosten_sehen();															// 0=None, 1=Intern, 2=Extern or 3=Both

	$app=false;
	if($mn == 425)	// Dummy Matrix
		$app=prjTableHasApproval();

	$lta=array(1=>'literal_5', 'literal_10', 'literal_15', 'literal_20', 'literal_text');

	// Read fields from DB
	$sql  = "SELECT m.eingabefeld, m.spalte, m.anzeigeart, m.anzeigegroesse1, m.horausr, m.spaltenanzahl, m.defaultwert, m.spezial,";
	$sql .= " f.feldname, f.feldtyp,";
	$sql .= " t.literal_5, t.literal_10, t.literal_15, t.literal_20, t.literal_text";
	$sql .= " FROM maskenfelder m, felder f, texte t";
	$sql .= " WHERE m.nummer=".$mn." AND m.zeile=1 AND f.id=m.eingabefeld AND f.mandanten_id=0 AND t.id_referenz=m.eingabefeld AND t.id_ref_art=3 AND id_sprache=".$GLOBALS['fxpglobals']['lang'];
	if(!$app)
		$sql .= " AND m.eingabefeld<>1285";
	$sql .= " ORDER BY m.spalte";
	$arr=db_values($sql,'*');
//fxDebug($arr,$sql);

	// User fields in Matrix
	if($mn == 425)
	{
		getProjectUserFields();
		if(fxIsArray($GLOBALS['ufields']))
		{
			$soa=sizeof($arr);
			foreach($GLOBALS['ufields'] as $u)
			{
				$arr[$soa]=$u;
				$soa++;
			}
		}
	}

	// Budgetfields
	$bfa=array('Epbillcat','Embillcat','Bbtype','Iihrframe','Iihrplan','Iihrused','Eehrframe','Eehrplan','Eehrused','Eehrinv','Iimrframe','Iimrplan','Iimrused','Eemrframe','Eemrplan','Eemrused','Eemrinv','Ipp','Esp');

	// Matrix literals and fields
	$mla=array();
	$mfa=array();
	$oid='';
	$ocn=0;
	if(fxIsArray($arr))
	{
		foreach($arr as $a)
		{
			$ncn=(int)($a['spalte']/100);
			if(strlen($oid) && $ocn && ($ocn != $ncn))
				$mfa[$oid] .= '***';

			if($a['eingabefeld'] == 240)
			{
				$resp_role_names=get_resp_role_names();
				if(fxIsArray($resp_role_names))
				{
					$ocn=$ncn;
					$oid=$a['eingabefeld'];
					$pvrid=500;
					$sirrn=sizeof($resp_role_names);
					foreach($resp_role_names as $resp_role)
					{
						$mla[$pvrid]=$resp_role['mafaehigkeit_name'];

						$mfa[$pvrid]='pvr'.$resp_role['mafaehigkeit_id'].'|5|8|200||0|';

						$oid=$pvrid;
						$pvrid++;
					}
				}
			}
			else
			{
				$ocn=$ncn;
				$oid=$a['eingabefeld'];
				$lt=(int)$a['anzeigegroesse1'];
				if(!$lt)
					$lt=4;
				else
					$lt=max(1,min(5,(int)$a['anzeigegroesse1']));

				// ...Literal
				$mla[$a['eingabefeld']]=$a[$lta[$lt]];

				// ...Field
				// Matrix fields: RO=ReadOnly, DT=Date, SL=Select, DR=Duration, CH=Cost per hour, BU=Budget, TX=Text, TA=Textarea
				$ft=getExtendedTypeOfField($a['feldname'],$a['feldtyp']);
				$at=$a['anzeigeart'];

				$fn=$a['spezial'];
				if(!strlen($fn))
					$fn=$a['feldname'];

				$align=$a['horausr'];
				if($align == 'left')
					$align='';

				$def=(int)$a['defaultwert'];

				// Check if budget field is allowed
				foreach($bfa as $bfs)
				{
					$bt=substr($bfs,0,1);
					$bf=substr($bfs,1);
					if($fn == $bf)
					{
						// Don't display cost fields
						$hide=false;
						if(!$cst || ($btype == 'N'))
							$hide=true;
						else if(($cst < 3) || ($btype != 'B'))
						{
							// Just display int. cost fields
							if(($bt == 'E') && ((($cst&2) == 0) || ($btype == 'I')))
								$hide=true;
							// Just display ext. cost fields
							if(($bt == 'I') && ((($cst&1) == 0) || ($btype == 'E')))
								$hide=true;
						}

						if($hide)
						{
							// ReadOnly
							if($at == 8)
								$ft=-1;
							$at=5;
							$def=2;	// Always disabled
						}
					}
				}

				$mfa[$a['eingabefeld']]=$fn.'|'.$ft.'|'.$at.'|'.(int)$a['spaltenanzahl'].'|'.$align.'|'.$def.'|';
			}
		}
	}

	$dfa=array('mla'=>$mla, 'mfa'=>$mfa);

	// Create minimum dummy session mask
	$GLOBALS['fx_mfields'][$mn]=array();
	$mfc=0;
	foreach($mfa as $fid => $fa)
	{
		$fd=explode('|',$fa);
		$GLOBALS['fx_mfields'][$mn][$mfc]=array(
			'zeile'			=> 1,
			'eingabefeld'	=> $fid,
			'feldname'		=> $fd[0],
			'defaultwert'	=> (int)$fd[5],
			'literal_value'	=> $mla[$fid]
		);
		$mfc++;
	}

	return $dfa;
}

/**
 * ???FUNCTION???
 *
 * @param various $fieldname - Mandatory parameter: ???PARAMETER???
 * @param various $fieldtype - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getExtendedTypeOfField($fieldname, $fieldtype)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$lfn=strtolower($fieldname);
	switch($lfn)
	{
		case 'aufwand_soll':
		case 'aufwand_ist':
		case 'max_zeit_aufw':
			$fieldtype=50;	// Durations
		break;

		case 'ek':
		case 'vk':
			$fieldtype=51;	// Hourly Costs
		break;
	}

	return $fieldtype;
}

/**
 * ???FUNCTION???
 */
function getProjectUserFields()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(fxIsArray($GLOBALS['ufields']))
		return;

	$GLOBALS['ufields']=array();
	$GLOBALS['utable']='';
	$GLOBALS['unumber']=0;

	$sql  = "SELECT tm.nummer, m.name_maske FROM trans_maske tm, masken m";
	$sql .= " WHERE tm.nummer<0 AND tm.transakt_id=23 AND m.nummer=tm.nummer AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$umt  = db_values($sql);
	if(fxIsArray($umt))
	{
		$GLOBALS['unumber']	= (int)$umt['nummer'];
		$GLOBALS['utable']	= strtolower(substr($umt['name_maske'], 1));

		// Read fields from DB
		$sql  = "SELECT m.eingabefeld, m.spalte, m.anzeigeart, m.anzeigegroesse1, m.horausr, m.spaltenanzahl, m.defaultwert, m.spezial,";
		$sql .= " f.feldname, f.feldtyp,";
		$sql .= " t.literal_5, t.literal_10, t.literal_15, t.literal_20, t.literal_text";
		$sql .= " FROM maskenfelder m, felder f, texte t";
		$sql .= " WHERE m.nummer=".$GLOBALS['unumber']." AND m.anzeigeart IN (6,7,8) AND f.id=m.eingabefeld AND f.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].") AND t.id_referenz=m.eingabefeld AND t.id_ref_art=3 AND id_sprache=".$GLOBALS['fxpglobals']['lang'];
		$sql .= " ORDER BY m.zeile, m.spalte";
		$uarr = db_values($sql, '*');
//fxDebug($uarr, $sql);
		if(fxIsArray($uarr))
		{
			$cid=10001;
			foreach($uarr as $u)
			{
				$uid=$u['eingabefeld'];
				if($uid < 0)
					$uid='n'.abs($uid);
				$GLOBALS['ufields'][$uid]=$u;

				// Type
				$GLOBALS['ufields'][$uid]['feldtyp']=getExtendedTypeOfField($u['feldname'], $u['feldtyp']);
				$GLOBALS['ufields'][$uid]['ftype']='S';
				if(($u['feldtyp'] == 7) || ($u['feldtyp'] == 10) || ($u['feldtyp'] == 14))
					$GLOBALS['ufields'][$uid]['ftype']='D';
				else if(($u['feldtyp'] == 6) || ($u['feldtyp'] == 11) || ($u['feldtyp'] == 16) || ($u['feldtyp'] == 17) || ($u['feldtyp'] == 51))
					$GLOBALS['ufields'][$uid]['ftype']='F';
				else if(($u['feldtyp'] == 2) || ($u['feldtyp'] == 4) || ($u['feldtyp'] == 5) || ($u['feldtyp'] == 8) || ($u['feldtyp'] == 12) || ($u['feldtyp'] == 14) || ($u['feldtyp'] == 50))
					$GLOBALS['ufields'][$uid]['ftype']='I';

				// No
				$GLOBALS['ufields'][$uid]['spalte']=$cid;
				$cid++;

				// Width
				if($u['feldtyp'] == 7)	// Date
				{
					$GLOBALS['ufields'][$uid]['spaltenanzahl']=100;
					$GLOBALS['ufields'][$uid]['anzeigegroesse1']=2;
				}
				else if(($u['ftype'] == 'F') || ($u['ftype'] == 'I'))	// Number
				{
					$GLOBALS['ufields'][$uid]['spaltenanzahl']=125;
					$GLOBALS['ufields'][$uid]['anzeigegroesse1']=3;
				}
				else
					$GLOBALS['ufields'][$uid]['spaltenanzahl']=200;

				// Name
				$GLOBALS['ufields'][$uid]['spezial']='puf'.$uid;

				// Default
				$GLOBALS['ufields'][$uid]['defaultwert']=0;
			}
		}
//fxDebug($GLOBALS['ufields'], '$ufields of table "'.$GLOBALS['utable'].'"');
	}
}

/**
 * ???FUNCTION???
 */
function maskClearLC()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$GLOBALS['fxptdata']['litf_'.$GLOBALS['_masknumber']]=array();
	$GLOBALS['fxptdata']['litc_'.$GLOBALS['_masknumber']]=array();
	$GLOBALS['fxptdata']['chkf_'.$GLOBALS['_masknumber']]=array();
}

/**
 * ???FUNCTION???
 *
 * @param various $mn        - Mandatory parameter: ???PARAMETER???
 * @param various $fieldname - Mandatory parameter: ???PARAMETER???
 * @param various $col       - Mandatory parameter: ???PARAMETER???
 * @param various $lit       - Mandatory parameter: ???PARAMETER???
 */
function maskAddCBColumn($mn,$fieldname,$col,$lit)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$GLOBALS['fxptdata']['litf_'.$mn][$fieldname]=$lit;
	$GLOBALS['fxptdata']['litc_'.$mn][$col]=$stl;
}

/**
 * $mno:		Mask number (int)
 * $fields:		Fields to integrate (array)
 * fieldname	=> literal
 * => header (array)
 * => data (array)
 *
 * @param various $mno    - Mandatory parameter:                  ???PARAMETER???
 * @param various $fields - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $clear  - Optional parameter (default = true):  ???PARAMETER???
 */
function maskAddNewListFields($mno,$fields,$clear=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mno=(string)(trim($mno));

	// Load actual mask files
	if(!fxIsArray($GLOBALS['fx_mfields'][$mn]))
		sessionMask($mn);
//fxDebug($GLOBALS['fx_mask'][$mno],'fx_mask['.$mno.']');
//fxDebug($GLOBALS['fx_mfields'][$mno],'fx_mfields['.$mno.']');
//fxDebug($GLOBALS['fx_mfieldsarray'][$mno],'fx_mfieldsarray['.$mno.']');
//fxDebug($GLOBALS['fx_mheaders'][$mno],'fx_mheaders['.$mno.']');

	// Extract mask infos
	$ln=0;
	$hfields=array();
	$hfieldc=array();
	$dfields=array();
	$dfieldc=array();
	$bfields=array();
	foreach($GLOBALS['fx_mfields'][$mno] as $k => $d)
	{
		$line=(int)$d['zeile'];
		$column=(int)$d['spalte'];
		if($line == 1)			// Header
		{
			$ak=sizeof($hfields);
			$hfields[$ak]=$d;
			$hfieldc[$column]=$ak;
		}
		else if($line >= 9000)	// Button
			$bfields[]=$d;
		else					// Data
		{
			$ak=sizeof($dfields);
			$dfields[$ak]=$d;
			$dfieldc[$column]=$ak;

			if(!$ln)
				$ln=$line;
		}
	}

	if(!$ln)
		$ln=10;
//echo('$ln='.$ln.'<br />');

	// Clear old additional fields
	if($clear)
	{
		$tdf=array();
		foreach($dfieldc as $column => $dk)
		{
			if((int)$dfields[$dk]['maskenfeldid'] === 0)
			{
				unset($dfields[$dk]);
				$hk=$hfieldc[$column];
				unset($hfields[$hk]);
				unset($hfieldc[$column]);
			}
			else
				$tdf[$column]=$dk;
		}
		$dfieldc=$tdf;
	}

	// Get max header/data column
	$nc=0;
	$fieldid=0;
	foreach($hfieldc as $column => $hk)
		$nc=max($nc,$column);
	foreach($dfieldc as $column => $dk)
	{
		$nc=max($nc,$column);
		$fieldid=max($fieldid, (int)$dfields[$dk]['id']);
	}
	$nc++;
	$fieldid=max(100000, $fieldid+1);
//echo('$nc='.$nc.', $fieldid='.$fieldid.'<br />');

	// Add new column(s)
	if(fxIsArray($fields))
	{
//fxDebug($fields,'$fields');
		$dummyfield=array(
			'nummer'				=> $mno,
			'maskenfeldid'			=> '0',		// ID of entry in mask field table
			'zeile'					=> '0',
			'spalte'				=> '0',
			'anzeigeart'			=> '0',		// Display type
			'eingabefeld'			=> '0',		// Field ID
			'anzeigegroesse1'		=> '0',
			'anzeigegroesse2'		=> '0',
			'muss'					=> '0',
			'horausr'				=> 'center',
			'vertausr'				=> 'middle',
			'spaltenanzahl'			=> '',
			'zeilenanzahl'			=> '',
			'spaltenbreite'			=> '',
			'zeilenhoehe'			=> '',
			'literal_overwrite'		=> '',
			'literal_value'			=> '',
			'defaultwert'			=> '',
			'spezial'				=> '',
			'idintab'				=> '',
			'zusatzparameter_int1'	=> '',
			'zusatzparameter_int2'	=> '',
			'zusatzparameter_txt'	=> '',
			'mandanten_id'			=> '0',
			'id'					=> '0',		// Masterfield ID
			'feldname'				=> '',
			'alias_zu'				=> '0',
			'feldtyp'				=> '1',		// Field type: 1=Text
			'laenge'				=> '0',
			'nachkomma'				=> '0',
			'html_grafik'			=> '',
			'texte_leer'			=> '0',
			'nachschlage'			=> '0',
			'nachschlage_art'		=> '0',
			'kosten'				=> '0',
			'id_text'				=> '0',
			'text_kurz'				=> '',
			'text_varlang'			=> '',
			'anmerkung'				=> '',
			'literal_5'				=> '',
			'literal_10'			=> '',
			'literal_15'			=> '',
			'literal_20'			=> '',
			'literal_text'			=> ''
		);
//fxDebug($dummyfield,'$dummyfield');

		foreach($fields as $fieldname => $fieldspec)
		{
			$hidden=false;
			if(substr($fieldname,0,1) == '-')
			{
				$hidden=true;
				$fieldname=substr($fieldname,1);
			}

			// Header
			if(!$hidden)
			{
				$hk=sizeof($hfields);
				$hfieldc[$nc]=$hk;
				$hfields[$hk]=$dummyfield;
				$hfields[$hk]['zeile']='1';
				$hfields[$hk]['spalte']=(string)$nc;
				$hfields[$hk]['anzeigeart']='20';	// 20=Literal_Text
				$hfields[$hk]['eingabefeld']=(string)$fieldid;
				$hfields[$hk]['literal_value']=$fieldspec['literal'];
				$hfields[$hk]['id']=(string)$fieldid;
				$hfields[$hk]['feldname']=$fieldname;
				$hfields[$hk]['literal_5']=substr($fieldspec['literal'],0,5);
				$hfields[$hk]['literal_10']=substr($fieldspec['literal'],0,10);
				$hfields[$hk]['literal_15']=substr($fieldspec['literal'],0,15);
				$hfields[$hk]['literal_20']=substr($fieldspec['literal'],0,20);
				$hfields[$hk]['literal_text']=$fieldspec['literal'];
				if(fxIsArray($fieldspec['header']))
				{
					foreach($fieldspec['header'] as $fk => $fv)
					{
						if(isset($hfields[$hk][$fk]))
							$hfields[$hk][$fk]=(string)$fv;
					}
				}
			}

			// Data
			$dk=sizeof($dfields);
			$dfieldc[$nc]=$dk;
			$dfields[$dk]=$dummyfield;
			$dfields[$dk]['zeile']=(string)$ln;
			$dfields[$dk]['spalte']=(string)$nc;
			$dfields[$dk]['eingabefeld']=(string)$fieldid;
			$dfields[$dk]['id']=(string)$fieldid;
			$dfields[$dk]['feldname']=$fieldname;
			if($hidden)
				$dfields[$dk]['anzeigeart']='10';	// 10=Hidden
			else
			{
				$dfields[$dk]['anzeigeart']='5';	// 5=Read-Only
				$dfields[$dk]['literal_value']=$fieldspec['literal'];
				$dfields[$dk]['literal_5']=substr($fieldspec['literal'],0,5);
				$dfields[$dk]['literal_10']=substr($fieldspec['literal'],0,10);
				$dfields[$dk]['literal_15']=substr($fieldspec['literal'],0,15);
				$dfields[$dk]['literal_20']=substr($fieldspec['literal'],0,20);
				$dfields[$dk]['literal_text']=$fieldspec['literal'];
				if(fxIsArray($fieldspec['data']))
				{
					foreach($fieldspec['data'] as $fk => $fv)
					{
						if(isset($dfields[$dk][$fk]))
							$dfields[$dk][$fk]=(string)$fv;
					}
				}
			}

			$nc++;
			$fieldid++;
		}
	}
//fxDebug($hfields,'Headers');
//fxDebug($dfields,'Datas');
//fxDebug($bfields,'Buttons');
//fxDebug($hfieldc,'Header Columns');
//fxDebug($dfieldc,'Data Columns');

	// Create new field array
	$mk=0;
	$GLOBALS['fx_mfields'][$mno]=array();
	// ...header column(s)
	foreach($hfieldc as $column => $hk)
	{
		$GLOBALS['fx_mfields'][$mno][$mk]=$hfields[$hk];
		$mk++;
	}
	// ...data column(s)
	foreach($dfieldc as $column => $dk)
	{
		$GLOBALS['fx_mfields'][$mno][$mk]=$dfields[$dk];
		$mk++;
	}
	// ...button(s)
	foreach($bfields as $ba)
	{
		$GLOBALS['fx_mfields'][$mno][$mk]=$ba;
		$mk++;
	}
//fxDebug($GLOBALS['fx_mfields'][$mno],'fx_mfields['.$mno.']');

	// Save mask
	sessionMask($mno, false, '', '', $GLOBALS['fx_mfields'][$mno]);
}

/**
 * ???FUNCTION???
 *
 * @param various $h   - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $add - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function maskHeadlineEntries($h, $add=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$he='&nbsp;&nbsp;<font class="s2 grey normal">(';
	if($add)
		$h .= $he.meldung(1330, false, '<!--anzahl-->').')</font>';	// Es wurden ... Einträge gefunden
	else
	{
		$hep=strpos($h, $he);
		if($hep)
			$h=trim(substr($h,0,$hep));
		$hrs=strpos($h, '<rtitle>');
		if($hrs !== false)
		{
			$h=substr($h,$hrs+8);
			$hre=strpos($h, '</rtitle>');
			if($hre !== false)
				$h=substr($h,0,$hre);
		}
		$h=str_replace('"', '&quot;', $h);
	}

	// Div's entfernen
	while(true)
	{
		$dsp=stripos($h,'<div');
		if($dsp === false)
			break;
		$dep=stripos($h,'div>',$dsp);
		if($dep === false)
			$h=trim(substr($h,0,$dsp));
		else
			$h=trim(substr($h,0,$dsp)).trim(substr($h,$dep+4));
	}
//echo('$h:<pre>'.fxHtmlEncode($h).'</pre>');

	return $h;
}

/**
 * ???FUNCTION???
 *
 * @param integer $mn     - Optional parameter (default = 0):     ???PARAMETER???
 * @param various $mwidth - Optional parameter (default = -1):    ???PARAMETER???
 * @param boolean $fv     - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function maskNoEntries($mn=0, $mwidth=-1, $fv=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mn=(int)$mn;
	if(!$mn)
		$mn=(int)$GLOBALS['_masknumber'];

	$mwidth=(int)$mwidth;
	if(!$mwidth)
	{
		$mwidth=$GLOBALS['miwidth'];
		if(!$mwidth)
			$mwidth=940;
		$mwidth -= 22;
	}
	$mwstyle='';
	if($mwidth > 0)
		$mwstyle='min-width:'.$mwidth.'px;';

	$tab='';
	if(isset($GLOBALS['mttabs']))
		$tab=$GLOBALS['mttabs'].'					';

	$simple=false;
	if(isset($GLOBALS['msk'.$mn.'_simplemessage']) && $GLOBALS['msk'.$mn.'_simplemessage'])
	{
		$simple=true;
		if(is_string($GLOBALS['msk'.$mn.'_simplemessage']))
			$smsg=$GLOBALS['msk'.$mn.'_simplemessage'];
		else
			$smsg=meldung(2960, true);	// Keine Einträge vorhanden!
		$mld_entries='<font class="grey" style="font-size:11pt;">'.$smsg.'</font>';
	}
	else
	{
		$mld_entries='<font id="mtef_'.$mn.'" class="s4 grey normal">'.meldung(712, true).'</font>';// Es wurden keine passenden Einträge gefunden!
		$mld_reasons=meldung(2763, true);// Mögliche Ursachen
		$mld_criteria=meldung(2764, true);// Daten liegen ausserhalb der Anzeigekriterien gemäß obiger Auswahlmaske!
	}

	$iopcl='';
	if(!$simple && (!isset($GLOBALS['app_wd']) || !$GLOBALS['app_wd']))
		$iopcl .= '<img id="mtei_'.$mn.'" src="'.$GLOBALS['gfxpath'].'fl_op.png" style="margin-right:8px;cursor:pointer;">';

	$div  = $tab.'<!-- DBI: Body div (start) -->'.$GLOBALS['nl'];
	$div .= $tab.'<div class="maskm" style="position:relative;'.$mwstyle.'white-space:normal;">'.$GLOBALS['nl'];
	$div .= $tab.'	<div id="mted_'.$mn.'" style="position:relative;text-align:center;"><br />'.$iopcl.$mld_entries.'<br /><br /></div>'.$GLOBALS['nl'];
	if(!$simple)
	{
		$div .= $tab.'	<div id="mtem_'.$mn.'" style="position:relative;text-align:center;display:none;">'.$GLOBALS['nl'];
		$div .= $tab.'		<hr class="fxhr"><br />'.$GLOBALS['nl'];
		$div .= $tab.'		<table align="center">'.$GLOBALS['nl'];
		$div .= $tab.'			<tr>'.$GLOBALS['nl'];
		$div .= $tab.'				<td align="center" colspan="2"><b class="darkgrey">'.$mld_reasons.':</b><br /><br /></td>'.$GLOBALS['nl'];
		$div .= $tab.'			</tr>'.$GLOBALS['nl'];
		$div .= $tab.'			<tr>'.$GLOBALS['nl'];
		$div .= $tab.'				<td valign="top">&middot;</td>'.$GLOBALS['nl'];
		$div .= $tab.'				<td valign="top" align="left">'.$mld_criteria.'</td>'.$GLOBALS['nl'];
		$div .= $tab.'			</tr>'.$GLOBALS['nl'];
		$ftt=filter_tooltip($fv, false);
		if(strlen($ftt))
		{
			$mld_filter=meldung(2765, true);// Daten wurden aufgrund der aktuell aktiven Filter nicht angezeigt!
			$div .= $tab.'			<tr>'.$GLOBALS['nl'];
			$div .= $tab.'				<td valign="top">&middot;</td>'.$GLOBALS['nl'];
			$div .= $tab.'				<td valign="top" align="left">'.$mld_filter.'<br /><br />'.$ftt.'</td>'.$GLOBALS['nl'];
			$div .= $tab.'			</tr>'.$GLOBALS['nl'];
		}
		$div .= $tab.'		</table>'.$GLOBALS['nl'];
		$div .= $tab.'	</div>'.$GLOBALS['nl'];
	}
	$div .= $tab.'</div>'.$GLOBALS['nl'];
	$div .= $tab.'<!-- DBI: Body div (end) -->'.$GLOBALS['nl'];

	return $div;
}

/**
 * ???FUNCTION???
 *
 * @param various $m - Mandatory parameter:                ???PARAMETER???
 * @param various $k - Mandatory parameter:                ???PARAMETER???
 * @param various $t - Optional parameter (default = -1):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getFieldTooltip($m, $k, $t=-1)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(($GLOBALS['fxpglobals']['tr'] > 0) && fxIsArray($GLOBALS['fx_mfields']) && fxIsArray($GLOBALS['fx_mfields'][$m]) && fxIsArray($GLOBALS['fx_mfields'][$m][$k]))
	{
		$t=(int)$t;
		if(($t < 0) && fxIsArray($GLOBALS['fx_mask']) && fxIsArray($GLOBALS['fx_mask'][$m]))
			$t=(int)$GLOBALS['fx_mask'][$m]['maskenart'];

		if(($t > 1) && ($t < 6))
		{
			$a=$GLOBALS['fx_mfields'][$m][$k];
			$f=$a['feldname'];

			// Tooltip im Programm festgelegt?
			if(isset($GLOBALS[$f.'_lit_tooltip']))
				return apos2quot($GLOBALS[$f.'_lit_tooltip']);

			// Tooltip speziell für diese Transaktion definiert?
			if(strlen($a['tooltip_tr']))
			{
				$dtta=explode("\n",$a['tooltip_tr']);
				foreach($dtta as $dtt)
				{
					$dtt=trim($dtt);
					$ltt=strlen($dtt);
					$fap=-1;
					for($p=0; $p<$ltt; $p++)
					{
						$o=ord(substr($dtt,$p,1));
						if((($o < 0x30) || ($o > 0x39)) && ($o != 0x7c))	// Erster Buchstabe ausser Pipe | gefunden
						{
							$fap=$p;
							break;
						}
					}
					if($fap > 0)
					{
						$tta=explode('|', trim(substr($dtt,0,$fap)));
						foreach($tta as $ttr)
						{
							$ttr=(int)trim($ttr);
							if($ttr == $GLOBALS['fxpglobals']['tr'])
								return apos2quot(trim(substr($dtt,$fap)));
						}
					}
				}
			}

			// Tooltip explizit deaktiviert?
			if(($a['anzeigegroesse1'] == -999) || ($a['anzeigegroesse2'] == -999))
				return '';

			// Literal holen
			$l=getLiteral($a);
			$fl=$l['lit'];
			// Tooltip Literal ermitteln
			if($a['anzeigegroesse2'] < 0)
			{
				$ag=min(6, max(abs($a['anzeigegroesse2']), 1));
				if($ag < 5)
					$tl='literal_'.($ag*5);
				else
					$tl='text_kurz';
			}
			else if($a['anzeigegroesse1'] < 0)
			{
				$ag=min(6, max(abs($a['anzeigegroesse1']), 1));
				if($ag < 5)
					$tl='literal_'.($ag*5);
				else
					$tl='text_kurz';
			}
			else if($d == 'literal_20')
				$tl='text_kurz';
			else
				$tl='literal_20';
			$tt=$a[$tl];

			// Tooltip und Literal gleich, dann kein Tooltip anzeigen
			if($tt === $fl)
				return '';
			return apos2quot($tt);
		}
	}

	return '';
}

/**
 * ???FUNCTION???
 *
 * @param various $a   - Mandatory parameter:                ???PARAMETER???
 * @param string  $fv  - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $rep - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $sid - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getLiteral($a, $fv='', $rep='', $sid='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$set=false;	// Literal set?
	$dis=true;	// Display?

	$lsp=false;	// Span?
	$lbo=false;	// Bold?
	$lcl=false;	// Class?
	$lco=false;	// Colon?
	$lbr=0;		// Break?

	$lit='';

	// Literal im Programm festgelegt?
	$f=$a['feldname'];
	if(!$set)
	{
		$o=$f.'_lit';
		if(isset($GLOBALS[$o]))
		{
			$set=true;
			$lit=trim($GLOBALS[$o]);
			if(!strlen($lit))
				$dis=false;
		}
	}

	// Literal speziell für diese Transaktion definiert?
	if(!$set && (strlen($a['literal_tr'])))
	{
		$dlta=explode("\n",$a['literal_tr']);
		foreach($dlta as $dlt)
		{
			$dlt=trim($dlt);
			$llt=strlen($dlt);
			$fap=-1;
			for($p=0; $p<$llt; $p++)
			{
				$o=ord(substr($dlt,$p,1));
				if((($o < 0x30) || ($o > 0x39)) && ($o != 0x7c))	// Erster Buchstabe ausser Pipe | gefunden
				{
					$fap=$p;
					break;
				}
			}
			if($fap > 0)
			{
				$lta=explode('|', trim(substr($dlt,0,$fap)));
				foreach($lta as $ltr)
				{
					$ltr=(int)trim($ltr);
					if($ltr == $GLOBALS['fxpglobals']['tr'])
					{
						$set=true;
						$lit=trim(substr($dlt,$fap));
						if(!strlen($lit))
							$dis=false;

						break 2;
					}
				}
			}
		}
	}

	if($rep !== '')
		$t=$GLOBALS['anzart_defines'][$rep];
	else
		$t=$GLOBALS['anzart_defines'][$a['anzeigeart']];

	// Literalgröße überschreiben?
	$n=$t;
	$ag=(int)$a['anzeigegroesse2'];
	if($ag > 0)
	{
		$lbo=true;
		$lcl=true;
		if($ag > 99)
			$lbr=2;
		if($ag < 5)
			$n='literal_'.($ag*5);
		else if($ag == 5)
			$n='literal_text';
		else if(($ag != 6) && $a['anmerkung'])
			$n='anmerkung';
		else
			$n='text_kurz';
	}
	else
	{
		$s=strtolower(trim($a['spezial']));

		// Prüfen, ob Literal in einen Span-Tag eingebunden werden soll
		if(($rep === '') && ($sid != 'header'))
		{
			$lsp=true;
			$lcl=true;
		}
		// Prüfen, ob Literal fett dargestellt werden soll
		$bold=$f.'_bold';
		if((isset($GLOBALS[$bold]) && ($GLOBALS[$bold] === true)) || (strlen($s) && ($s == 'b'))) // Fett
			$lbo=true;
		// Prüfen, ob Doppelpunkte angezeigt werden sollen
		$nc=$f.'_nc';
		if(($fv == ':') && (!isset($GLOBALS[$nc]) || (gettype($GLOBALS[$nc]) != 'boolean') || ($GLOBALS[$nc] != true))) // Doppelpunkte
		{
			$lco=true;
			$lbr=1;
		}
	}

	if(!$set)
		$lit=$a[$n];

	$tmp='';
	if($dis)
	{
		if($lsp)
			$tmp .= '<span';
		else if($lbo)
			$tmp .= '<b';
		if(strlen($tmp))
		{
			$tmp .= ' id="lit_'.$f.'"';
			if($lcl)
				$tmp .= ' class="lit"';
			$tmp .= '>';
		}
		if($lsp && $lbo)
			$tmp .= '<b id="lit_'.$f.'">';
		$tmp .= $lit;
		if($lco)
			$tmp .= ':';
		$tmp .= '&nbsp;';
		if($lbo)
			$tmp .= '</b>';
		if($lbr)
			$tmp .= str_repeat('<br />',$lbr);
		if($lsp)
			$tmp .= '</span>';
	}
//echo('$lit:<pre>'.$lit.'</pre>$tmp:<pre>'.fxHtmlEncode($tmp).'</pre><hr />');

	return array('lit'=>$lit, 'tmp'=>$tmp);
}

/**
 * ???FUNCTION???
 *
 * @param various $msk  - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $disp - Optional parameter (default = true):  ???PARAMETER???
 * @param integer $mnr  - Optional parameter (default = 0):     ???PARAMETER???
 * @param integer $mtr  - Optional parameter (default = 0):     ???PARAMETER???
 * @param integer $mlg  - Optional parameter (default = 0):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function makeScreenshotMask($msk, $disp=true, $mnr=0, $mtr=0, $mlg=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mnr=(int)$mnr;
	if($mnr < 0)
		$mnr=(int)$GLOBALS['_masknumber'];
	$mtr=(int)$mtr;
	if($mtr <= 0)
		$mtr=$GLOBALS['fxpglobals']['tr'];
	$mlg=(int)$mlg;
	if($mlg <= 0)
		$mlg=$GLOBALS['fxpglobals']['lang'];

	$autoscroll='';
	if(isset($GLOBALS['autoscroll']) && ($GLOBALS['autoscroll'] == 'm'.$GLOBALS['_masknumber']))
		$autoscroll='<div id="autoscroll" style="float:left;"></div>';

	$mha=maskHelpFile($mtr,$mnr,$mlg);
//fxDebug($mha, 'maskHelpFile($mtr='.$mtr.', $mnr='.$mnr.', $mlg='.$mlg.')');
	$mim=$GLOBALS['gfxpath'].'im_photo'.$mha['ex'].'.png';

	$smsk  = $autoscroll.'<table id="mt'.$mnr.'_rahmen" border="0" cellpadding="0" cellspacing="0" style="position:relative;float:none;"><tr><td align="left">';
	$smsk .= '<img id="fxphoto_'.$mnr.'" src="'.$mim.'" tooltip="Take Mask Screenshot: '.$mha['id'].'" style="position:absolute;top:0;right:-36px;cursor:pointer;display:none;">';
	$smsk .= '<a href="'.$mha['mfilepath'].'?'.$GLOBALS['datetime'].'" target="fxpmaskscreenshots" tooltip="Open Mask Screenshot: '.$mha['mfilepath'].'"><img id="fxphoto_'.$mnr.'_disp" src="'.$GLOBALS['gfxpath'].'im_enl.png" style="position:absolute;top:0;right:-62px;cursor:pointer;display:none;"></a>';
	$smsk .= $msk;
	$smsk .= '</td></table></table>'.$GLOBALS['nl'];

	if($disp)
		echo($smsk);
	else
		return $smsk;
}

/**
 * ???FUNCTION???
 *
 * @param integer $mnr - Optional parameter (default = 0): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fxGetMaskWidth($mnr=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mnr=(int)$mnr;
	if(!$mnr)
		$mnr=(int)$GLOBALS['_masknumber'];

	$width=0;
	if($mnr)
		$width=(int)db_value("SELECT breite FROM masken WHERE nummer=".$mnr." AND mandanten_id=0");
	if(!$width)
		$width=940;

	return $width;
}

/**
 * ???FUNCTION???
 *
 * @param integer $mnr   - Optional parameter (default = 0):      ???PARAMETER???
 * @param double  $size  - Optional parameter (default = 12.0):   ???PARAMETER???
 * @param string  $style - Optional parameter (default = 'bold'): ???PARAMETER???
 * @param string  $unit  - Optional parameter (default = 'px'):   ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fxGetMaskHeaderWidths($mnr=0, $size=12.0, $style='bold', $unit='px')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$cwa=array();

	$mnr=(int)$mnr;
	if(!$mnr)
		$mnr=(int)$GLOBALS['_masknumber'];
	if($mnr)
	{
		if(!fxIsArray($GLOBALS['fx_mfields'][$mnr]))
			sessionMask($mnr);
		if(fxIsArray($GLOBALS['fx_mfields'][$mnr]))
		{
//debug2($GLOBALS['fx_mfields'][$mnr],'fx_mfields['.$mnr.']');
			foreach($GLOBALS['fx_mfields'][$mnr] as $mca)
			{
				if($mca['zeile'] == 1)
					$cwa[strtolower($mca['feldname'])]=8+fxGetHelveticaStringWidth($mca['literal_value'], $size, $style, $unit);
			}
		}
	}

	return $cwa;
}

/**
 * ???FUNCTION???
 *
 * @param various $str   - Mandatory parameter:                      ???PARAMETER???
 * @param double  $size  - Optional parameter (default = 8.0):       ???PARAMETER???
 * @param string  $style - Optional parameter (default = 'normal'):  ???PARAMETER???
 * @param string  $unit  - Optional parameter (default = 'pt'):      ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fxGetHelveticaStringWidth($str, $size=8.0, $style='normal', $unit='pt')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$str=(string)$str;
	$unit=substr(strtolower(trim($unit)),0,2);

	// Style
	$bold=false;
	if(stripos($style,'b') !== false)
		$bold=true;
	$italic=false;
	if(stripos($style,'i') !== false)
		$italic=true;

	// Font width array for: Helvetica
	// ...bold+italic
	if($bold && $italic)
	{
		$acw=523;	// Average width for "undefined" characters
		$fwa=array(
			chr(0)=>278,chr(1)=>278,chr(2)=>278,chr(3)=>278,chr(4)=>278,chr(5)=>278,chr(6)=>278,chr(7)=>278,chr(8)=>278,chr(9)=>278,chr(10)=>278,chr(11)=>278,chr(12)=>278,chr(13)=>278,chr(14)=>278,chr(15)=>278,chr(16)=>278,chr(17)=>278,chr(18)=>278,chr(19)=>278,chr(20)=>278,chr(21)=>278,
			chr(22)=>278,chr(23)=>278,chr(24)=>278,chr(25)=>278,chr(26)=>278,chr(27)=>278,chr(28)=>278,chr(29)=>278,chr(30)=>278,chr(31)=>278,' '=>278,'!'=>333,'"'=>474,'#'=>556,'$'=>556,'%'=>889,'&'=>722,'\''=>238,'('=>333,')'=>333,'*'=>389,'+'=>584,
			','=>278,'-'=>333,'.'=>278,'/'=>278,'0'=>556,'1'=>556,'2'=>556,'3'=>556,'4'=>556,'5'=>556,'6'=>556,'7'=>556,'8'=>556,'9'=>556,':'=>333,';'=>333,'<'=>584,'='=>584,'>'=>584,'?'=>611,'@'=>975,'A'=>722,
			'B'=>722,'C'=>722,'D'=>722,'E'=>667,'F'=>611,'G'=>778,'H'=>722,'I'=>278,'J'=>556,'K'=>722,'L'=>611,'M'=>833,'N'=>722,'O'=>778,'P'=>667,'Q'=>778,'R'=>722,'S'=>667,'T'=>611,'U'=>722,'V'=>667,'W'=>944,
			'X'=>667,'Y'=>667,'Z'=>611,'['=>333,'\\'=>278,']'=>333,'^'=>584,'_'=>556,'`'=>333,'a'=>556,'b'=>611,'c'=>556,'d'=>611,'e'=>556,'f'=>333,'g'=>611,'h'=>611,'i'=>278,'j'=>278,'k'=>556,'l'=>278,'m'=>889,
			'n'=>611,'o'=>611,'p'=>611,'q'=>611,'r'=>389,'s'=>556,'t'=>333,'u'=>611,'v'=>556,'w'=>778,'x'=>556,'y'=>556,'z'=>500,'{'=>389,'|'=>280,'}'=>389,'~'=>584,chr(127)=>350,chr(128)=>556,chr(129)=>350,chr(130)=>278,chr(131)=>556,
			chr(132)=>500,chr(133)=>1000,chr(134)=>556,chr(135)=>556,chr(136)=>333,chr(137)=>1000,chr(138)=>667,chr(139)=>333,chr(140)=>1000,chr(141)=>350,chr(142)=>611,chr(143)=>350,chr(144)=>350,chr(145)=>278,chr(146)=>278,chr(147)=>500,chr(148)=>500,chr(149)=>350,chr(150)=>556,chr(151)=>1000,chr(152)=>333,chr(153)=>1000,
			chr(154)=>556,chr(155)=>333,chr(156)=>944,chr(157)=>350,chr(158)=>500,chr(159)=>667,chr(160)=>278,chr(161)=>333,chr(162)=>556,chr(163)=>556,chr(164)=>556,chr(165)=>556,chr(166)=>280,chr(167)=>556,chr(168)=>333,chr(169)=>737,chr(170)=>370,chr(171)=>556,chr(172)=>584,chr(173)=>333,chr(174)=>737,chr(175)=>333,
			chr(176)=>400,chr(177)=>584,chr(178)=>333,chr(179)=>333,chr(180)=>333,chr(181)=>611,chr(182)=>556,chr(183)=>278,chr(184)=>333,chr(185)=>333,chr(186)=>365,chr(187)=>556,chr(188)=>834,chr(189)=>834,chr(190)=>834,chr(191)=>611,chr(192)=>722,chr(193)=>722,chr(194)=>722,chr(195)=>722,chr(196)=>722,chr(197)=>722,
			chr(198)=>1000,chr(199)=>722,chr(200)=>667,chr(201)=>667,chr(202)=>667,chr(203)=>667,chr(204)=>278,chr(205)=>278,chr(206)=>278,chr(207)=>278,chr(208)=>722,chr(209)=>722,chr(210)=>778,chr(211)=>778,chr(212)=>778,chr(213)=>778,chr(214)=>778,chr(215)=>584,chr(216)=>778,chr(217)=>722,chr(218)=>722,chr(219)=>722,
			chr(220)=>722,chr(221)=>667,chr(222)=>667,chr(223)=>611,chr(224)=>556,chr(225)=>556,chr(226)=>556,chr(227)=>556,chr(228)=>556,chr(229)=>556,chr(230)=>889,chr(231)=>556,chr(232)=>556,chr(233)=>556,chr(234)=>556,chr(235)=>556,chr(236)=>278,chr(237)=>278,chr(238)=>278,chr(239)=>278,chr(240)=>611,chr(241)=>611,
			chr(242)=>611,chr(243)=>611,chr(244)=>611,chr(245)=>611,chr(246)=>611,chr(247)=>584,chr(248)=>611,chr(249)=>611,chr(250)=>611,chr(251)=>611,chr(252)=>611,chr(253)=>556,chr(254)=>611,chr(255)=>556
		);
	}
	// ...bold
	else if($bold)
	{
		$acw=523;	// Average width for "undefined" characters
		$fwa=array(
			chr(0)=>278,chr(1)=>278,chr(2)=>278,chr(3)=>278,chr(4)=>278,chr(5)=>278,chr(6)=>278,chr(7)=>278,chr(8)=>278,chr(9)=>278,chr(10)=>278,chr(11)=>278,chr(12)=>278,chr(13)=>278,chr(14)=>278,chr(15)=>278,chr(16)=>278,chr(17)=>278,chr(18)=>278,chr(19)=>278,chr(20)=>278,chr(21)=>278,
			chr(22)=>278,chr(23)=>278,chr(24)=>278,chr(25)=>278,chr(26)=>278,chr(27)=>278,chr(28)=>278,chr(29)=>278,chr(30)=>278,chr(31)=>278,' '=>278,'!'=>333,'"'=>474,'#'=>556,'$'=>556,'%'=>889,'&'=>722,'\''=>238,'('=>333,')'=>333,'*'=>389,'+'=>584,
			','=>278,'-'=>333,'.'=>278,'/'=>278,'0'=>556,'1'=>556,'2'=>556,'3'=>556,'4'=>556,'5'=>556,'6'=>556,'7'=>556,'8'=>556,'9'=>556,':'=>333,';'=>333,'<'=>584,'='=>584,'>'=>584,'?'=>611,'@'=>975,'A'=>722,
			'B'=>722,'C'=>722,'D'=>722,'E'=>667,'F'=>611,'G'=>778,'H'=>722,'I'=>278,'J'=>556,'K'=>722,'L'=>611,'M'=>833,'N'=>722,'O'=>778,'P'=>667,'Q'=>778,'R'=>722,'S'=>667,'T'=>611,'U'=>722,'V'=>667,'W'=>944,
			'X'=>667,'Y'=>667,'Z'=>611,'['=>333,'\\'=>278,']'=>333,'^'=>584,'_'=>556,'`'=>333,'a'=>556,'b'=>611,'c'=>556,'d'=>611,'e'=>556,'f'=>333,'g'=>611,'h'=>611,'i'=>278,'j'=>278,'k'=>556,'l'=>278,'m'=>889,
			'n'=>611,'o'=>611,'p'=>611,'q'=>611,'r'=>389,'s'=>556,'t'=>333,'u'=>611,'v'=>556,'w'=>778,'x'=>556,'y'=>556,'z'=>500,'{'=>389,'|'=>280,'}'=>389,'~'=>584,chr(127)=>350,chr(128)=>556,chr(129)=>350,chr(130)=>278,chr(131)=>556,
			chr(132)=>500,chr(133)=>1000,chr(134)=>556,chr(135)=>556,chr(136)=>333,chr(137)=>1000,chr(138)=>667,chr(139)=>333,chr(140)=>1000,chr(141)=>350,chr(142)=>611,chr(143)=>350,chr(144)=>350,chr(145)=>278,chr(146)=>278,chr(147)=>500,chr(148)=>500,chr(149)=>350,chr(150)=>556,chr(151)=>1000,chr(152)=>333,chr(153)=>1000,
			chr(154)=>556,chr(155)=>333,chr(156)=>944,chr(157)=>350,chr(158)=>500,chr(159)=>667,chr(160)=>278,chr(161)=>333,chr(162)=>556,chr(163)=>556,chr(164)=>556,chr(165)=>556,chr(166)=>280,chr(167)=>556,chr(168)=>333,chr(169)=>737,chr(170)=>370,chr(171)=>556,chr(172)=>584,chr(173)=>333,chr(174)=>737,chr(175)=>333,
			chr(176)=>400,chr(177)=>584,chr(178)=>333,chr(179)=>333,chr(180)=>333,chr(181)=>611,chr(182)=>556,chr(183)=>278,chr(184)=>333,chr(185)=>333,chr(186)=>365,chr(187)=>556,chr(188)=>834,chr(189)=>834,chr(190)=>834,chr(191)=>611,chr(192)=>722,chr(193)=>722,chr(194)=>722,chr(195)=>722,chr(196)=>722,chr(197)=>722,
			chr(198)=>1000,chr(199)=>722,chr(200)=>667,chr(201)=>667,chr(202)=>667,chr(203)=>667,chr(204)=>278,chr(205)=>278,chr(206)=>278,chr(207)=>278,chr(208)=>722,chr(209)=>722,chr(210)=>778,chr(211)=>778,chr(212)=>778,chr(213)=>778,chr(214)=>778,chr(215)=>584,chr(216)=>778,chr(217)=>722,chr(218)=>722,chr(219)=>722,
			chr(220)=>722,chr(221)=>667,chr(222)=>667,chr(223)=>611,chr(224)=>556,chr(225)=>556,chr(226)=>556,chr(227)=>556,chr(228)=>556,chr(229)=>556,chr(230)=>889,chr(231)=>556,chr(232)=>556,chr(233)=>556,chr(234)=>556,chr(235)=>556,chr(236)=>278,chr(237)=>278,chr(238)=>278,chr(239)=>278,chr(240)=>611,chr(241)=>611,
			chr(242)=>611,chr(243)=>611,chr(244)=>611,chr(245)=>611,chr(246)=>611,chr(247)=>584,chr(248)=>611,chr(249)=>611,chr(250)=>611,chr(251)=>611,chr(252)=>611,chr(253)=>556,chr(254)=>611,chr(255)=>556
		);
	}
	// ...italic
	else if($italic)
	{
		$acw=523;	// Average width for "undefined" characters
		$fwa=array(
			chr(0)=>278,chr(1)=>278,chr(2)=>278,chr(3)=>278,chr(4)=>278,chr(5)=>278,chr(6)=>278,chr(7)=>278,chr(8)=>278,chr(9)=>278,chr(10)=>278,chr(11)=>278,chr(12)=>278,chr(13)=>278,chr(14)=>278,chr(15)=>278,chr(16)=>278,chr(17)=>278,chr(18)=>278,chr(19)=>278,chr(20)=>278,chr(21)=>278,
			chr(22)=>278,chr(23)=>278,chr(24)=>278,chr(25)=>278,chr(26)=>278,chr(27)=>278,chr(28)=>278,chr(29)=>278,chr(30)=>278,chr(31)=>278,' '=>278,'!'=>333,'"'=>474,'#'=>556,'$'=>556,'%'=>889,'&'=>722,'\''=>238,'('=>333,')'=>333,'*'=>389,'+'=>584,
			','=>278,'-'=>333,'.'=>278,'/'=>278,'0'=>556,'1'=>556,'2'=>556,'3'=>556,'4'=>556,'5'=>556,'6'=>556,'7'=>556,'8'=>556,'9'=>556,':'=>333,';'=>333,'<'=>584,'='=>584,'>'=>584,'?'=>611,'@'=>975,'A'=>722,
			'B'=>722,'C'=>722,'D'=>722,'E'=>667,'F'=>611,'G'=>778,'H'=>722,'I'=>278,'J'=>556,'K'=>722,'L'=>611,'M'=>833,'N'=>722,'O'=>778,'P'=>667,'Q'=>778,'R'=>722,'S'=>667,'T'=>611,'U'=>722,'V'=>667,'W'=>944,
			'X'=>667,'Y'=>667,'Z'=>611,'['=>333,'\\'=>278,']'=>333,'^'=>584,'_'=>556,'`'=>333,'a'=>556,'b'=>611,'c'=>556,'d'=>611,'e'=>556,'f'=>333,'g'=>611,'h'=>611,'i'=>278,'j'=>278,'k'=>556,'l'=>278,'m'=>889,
			'n'=>611,'o'=>611,'p'=>611,'q'=>611,'r'=>389,'s'=>556,'t'=>333,'u'=>611,'v'=>556,'w'=>778,'x'=>556,'y'=>556,'z'=>500,'{'=>389,'|'=>280,'}'=>389,'~'=>584,chr(127)=>350,chr(128)=>556,chr(129)=>350,chr(130)=>278,chr(131)=>556,
			chr(132)=>500,chr(133)=>1000,chr(134)=>556,chr(135)=>556,chr(136)=>333,chr(137)=>1000,chr(138)=>667,chr(139)=>333,chr(140)=>1000,chr(141)=>350,chr(142)=>611,chr(143)=>350,chr(144)=>350,chr(145)=>278,chr(146)=>278,chr(147)=>500,chr(148)=>500,chr(149)=>350,chr(150)=>556,chr(151)=>1000,chr(152)=>333,chr(153)=>1000,
			chr(154)=>556,chr(155)=>333,chr(156)=>944,chr(157)=>350,chr(158)=>500,chr(159)=>667,chr(160)=>278,chr(161)=>333,chr(162)=>556,chr(163)=>556,chr(164)=>556,chr(165)=>556,chr(166)=>280,chr(167)=>556,chr(168)=>333,chr(169)=>737,chr(170)=>370,chr(171)=>556,chr(172)=>584,chr(173)=>333,chr(174)=>737,chr(175)=>333,
			chr(176)=>400,chr(177)=>584,chr(178)=>333,chr(179)=>333,chr(180)=>333,chr(181)=>611,chr(182)=>556,chr(183)=>278,chr(184)=>333,chr(185)=>333,chr(186)=>365,chr(187)=>556,chr(188)=>834,chr(189)=>834,chr(190)=>834,chr(191)=>611,chr(192)=>722,chr(193)=>722,chr(194)=>722,chr(195)=>722,chr(196)=>722,chr(197)=>722,
			chr(198)=>1000,chr(199)=>722,chr(200)=>667,chr(201)=>667,chr(202)=>667,chr(203)=>667,chr(204)=>278,chr(205)=>278,chr(206)=>278,chr(207)=>278,chr(208)=>722,chr(209)=>722,chr(210)=>778,chr(211)=>778,chr(212)=>778,chr(213)=>778,chr(214)=>778,chr(215)=>584,chr(216)=>778,chr(217)=>722,chr(218)=>722,chr(219)=>722,
			chr(220)=>722,chr(221)=>667,chr(222)=>667,chr(223)=>611,chr(224)=>556,chr(225)=>556,chr(226)=>556,chr(227)=>556,chr(228)=>556,chr(229)=>556,chr(230)=>889,chr(231)=>556,chr(232)=>556,chr(233)=>556,chr(234)=>556,chr(235)=>556,chr(236)=>278,chr(237)=>278,chr(238)=>278,chr(239)=>278,chr(240)=>611,chr(241)=>611,
			chr(242)=>611,chr(243)=>611,chr(244)=>611,chr(245)=>611,chr(246)=>611,chr(247)=>584,chr(248)=>611,chr(249)=>611,chr(250)=>611,chr(251)=>611,chr(252)=>611,chr(253)=>556,chr(254)=>611,chr(255)=>556
		);
	}
	// ...normal
	else
	{
		$acw=507;	// Average width for "undefined" characters
		$fwa=array(
			chr(0)=>278,chr(1)=>278,chr(2)=>278,chr(3)=>278,chr(4)=>278,chr(5)=>278,chr(6)=>278,chr(7)=>278,chr(8)=>278,chr(9)=>278,chr(10)=>278,chr(11)=>278,chr(12)=>278,chr(13)=>278,chr(14)=>278,chr(15)=>278,chr(16)=>278,chr(17)=>278,chr(18)=>278,chr(19)=>278,chr(20)=>278,chr(21)=>278,
			chr(22)=>278,chr(23)=>278,chr(24)=>278,chr(25)=>278,chr(26)=>278,chr(27)=>278,chr(28)=>278,chr(29)=>278,chr(30)=>278,chr(31)=>278,' '=>278,'!'=>278,'"'=>355,'#'=>556,'$'=>556,'%'=>889,'&'=>667,'\''=>191,'('=>333,')'=>333,'*'=>389,'+'=>584,
			','=>278,'-'=>333,'.'=>278,'/'=>278,'0'=>556,'1'=>556,'2'=>556,'3'=>556,'4'=>556,'5'=>556,'6'=>556,'7'=>556,'8'=>556,'9'=>556,':'=>278,';'=>278,'<'=>584,'='=>584,'>'=>584,'?'=>556,'@'=>1015,'A'=>667,
			'B'=>667,'C'=>722,'D'=>722,'E'=>667,'F'=>611,'G'=>778,'H'=>722,'I'=>278,'J'=>500,'K'=>667,'L'=>556,'M'=>833,'N'=>722,'O'=>778,'P'=>667,'Q'=>778,'R'=>722,'S'=>667,'T'=>611,'U'=>722,'V'=>667,'W'=>944,
			'X'=>667,'Y'=>667,'Z'=>611,'['=>278,'\\'=>278,']'=>278,'^'=>469,'_'=>556,'`'=>333,'a'=>556,'b'=>556,'c'=>500,'d'=>556,'e'=>556,'f'=>278,'g'=>556,'h'=>556,'i'=>222,'j'=>222,'k'=>500,'l'=>222,'m'=>833,
			'n'=>556,'o'=>556,'p'=>556,'q'=>556,'r'=>333,'s'=>500,'t'=>278,'u'=>556,'v'=>500,'w'=>722,'x'=>500,'y'=>500,'z'=>500,'{'=>334,'|'=>260,'}'=>334,'~'=>584,chr(127)=>350,chr(128)=>556,chr(129)=>350,chr(130)=>222,chr(131)=>556,
			chr(132)=>333,chr(133)=>1000,chr(134)=>556,chr(135)=>556,chr(136)=>333,chr(137)=>1000,chr(138)=>667,chr(139)=>333,chr(140)=>1000,chr(141)=>350,chr(142)=>611,chr(143)=>350,chr(144)=>350,chr(145)=>222,chr(146)=>222,chr(147)=>333,chr(148)=>333,chr(149)=>350,chr(150)=>556,chr(151)=>1000,chr(152)=>333,chr(153)=>1000,
			chr(154)=>500,chr(155)=>333,chr(156)=>944,chr(157)=>350,chr(158)=>500,chr(159)=>667,chr(160)=>278,chr(161)=>333,chr(162)=>556,chr(163)=>556,chr(164)=>556,chr(165)=>556,chr(166)=>260,chr(167)=>556,chr(168)=>333,chr(169)=>737,chr(170)=>370,chr(171)=>556,chr(172)=>584,chr(173)=>333,chr(174)=>737,chr(175)=>333,
			chr(176)=>400,chr(177)=>584,chr(178)=>333,chr(179)=>333,chr(180)=>333,chr(181)=>556,chr(182)=>537,chr(183)=>278,chr(184)=>333,chr(185)=>333,chr(186)=>365,chr(187)=>556,chr(188)=>834,chr(189)=>834,chr(190)=>834,chr(191)=>611,chr(192)=>667,chr(193)=>667,chr(194)=>667,chr(195)=>667,chr(196)=>667,chr(197)=>667,
			chr(198)=>1000,chr(199)=>722,chr(200)=>667,chr(201)=>667,chr(202)=>667,chr(203)=>667,chr(204)=>278,chr(205)=>278,chr(206)=>278,chr(207)=>278,chr(208)=>722,chr(209)=>722,chr(210)=>778,chr(211)=>778,chr(212)=>778,chr(213)=>778,chr(214)=>778,chr(215)=>584,chr(216)=>778,chr(217)=>722,chr(218)=>722,chr(219)=>722,
			chr(220)=>722,chr(221)=>667,chr(222)=>667,chr(223)=>611,chr(224)=>556,chr(225)=>556,chr(226)=>556,chr(227)=>556,chr(228)=>556,chr(229)=>556,chr(230)=>889,chr(231)=>500,chr(232)=>556,chr(233)=>556,chr(234)=>556,chr(235)=>556,chr(236)=>278,chr(237)=>278,chr(238)=>278,chr(239)=>278,chr(240)=>556,chr(241)=>556,
			chr(242)=>556,chr(243)=>556,chr(244)=>556,chr(245)=>556,chr(246)=>556,chr(247)=>584,chr(248)=>611,chr(249)=>556,chr(250)=>556,chr(251)=>556,chr(252)=>556,chr(253)=>500,chr(254)=>556,chr(255)=>500
		);
	}

	// Font size + scale (in unit)
	$size=(double)$size;
	if($unit == 'px')		// px (for HTML)
		$scale=0.745;
	else if($unit == 'mm')	// millimeter
		$scale=2.834646;
	else if($unit == 'cm')	// centimeter
		$scale=28.346457;
	else if($unit == 'in')	// inch
		$scale=72.0;
	else					// pt (for PDF=default)
		$scale=1.0;
	$fsize=$size/$scale;

	$sa=explode('{nl}', strip_tags(strtr($str, array('<br />'=>'{nl}', '<br>'=>'{nl}', '&nbsp;'=>chr(160)))));
//debug2($sa, '$sa: $str='.$str);

	// Get width
	$width=0;
	foreach($sa as $sstr)
	{
		$slen=strlen($sstr);
		$swidth=0;
		for($i=0; $i<$slen; $i++)
		{
			if(isset($fwa[$sstr[$i]]))
				$swidth += $fwa[$sstr[$i]];
			else
				$swidth += $acw;
		}
		$width=max($width,$swidth);
	}

	// Calculated width
	$cwidth=$width*$fsize/1000;
	if($unit == 'px')
		$cwidth=round($cwidth,0);

//echo('<b>fxGetHelveticaStringWidth</b><br /><font style="font-family:Helvetica;font-size:8pt;background:#006b9f;color:#fff;">'.htmlEncode($str).'</font><br />$style='.$style.', $unit='.$unit.', $slen='.$slen.', $width='.$width.'<br />$size='.$size.', $scale='.$scale.', $fsize='.$fsize.'<br />&rarr; $cwidth=<b>'.$cwidth.'</b><hr />');
	return $cwidth;
}

/**
 * Get all mask field names and return as an array
 *
 * @param array   $mn - Mandatory parameter:                            Mask number
 * @param integer $with_hidden - Optional parameter (default = false):  Get hidden fields?
 * @param array   $fields - Optional parameter (default = array():      Set special fields?
 *
 * @return array Mask field names as values
 */
function fxf_getMaskFields($mn, $with_hidden=false, $fields=array())
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mn=(int)$mn;
	$ma=array();

	// Mask type: 1=Single, 2=List (Single), 3=List (Total), 4=Report (Single) + 5=Report (Total)
	$sql="SELECT maskenart FROM masken WHERE nummer=".$mn." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
	$mt=(int)db_value($sql);
//fxDebug($mt,$sql, 0);

	// Mask fields
	if(($mt > 0) && ($mt < 6))
	{
		$sql  = "SELECT m.zeile, m.spalte, m.eingabefeld, m.anzeigeart, f.feldname";
		$sql .= " FROM maskenfelder m INNER JOIN felder f ON (f.id=m.eingabefeld AND f.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client']."))";
		$sql .= " WHERE m.nummer=".$mn." AND m.zeile<9000";
		if(!$with_hidden)
			$sql .= " AND m.anzeigeart<>10";
		if(($mt == 2) || ($mt == 3))
			$sql .= " AND m.zeile>1";
		$sql .= " ORDER BY m.zeile, m.spalte";
		$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql, 0);
		if(fxIsArray($tmp))
		{
			$maa=array();
			foreach($tmp as $t)
			{
				$maf=$t['feldname'];
				if(!isset($maa[$maf]))
				{
					if(fxIsArray($fields) && isset($fields[$maf]))
						$ma[]=$maf.'|'.$fields[$maf];
					else
						$ma[]=$maf;
				}
			}
		}
	}

	return $ma;
}

/**
 * Insert and empty line into the global field array variable $GLOBALS['fxptdata']['lvalues']
 *
 * @param array   $fa - Mandatory parameter:                  Field array containing all field names as values
 * @param integer $mc - Mandatory parameter:                  Mask counter
 * @param integer $ln - Mandatory parameter:                  Line number
 * @param string  $cv - Optional parameter (default = '~|'):  String value to be placed in each column
 */
function fxf_maskEmptyLine($fa, $mc=-1, $ln=-1, $cv='~|')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mc=(int)$mc;
	$ln=(int)$ln;

	if(fxIsArray($fa) && ($mc >= 0) && ($ln >= 0))
	{
		if(!fxIsArray($GLOBALS['fxptdata']['lvalues']))
			$GLOBALS['fxptdata']['lvalues']=array();
		if(!fxIsArray($GLOBALS['fxptdata']['lvalues'][$mc]))
			$GLOBALS['fxptdata']['lvalues'][$mc]=array();
		if(!fxIsArray($GLOBALS['fxptdata']['lvalues'][$mc][$ln]))
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]=array();

		foreach($fa as $fn)
		{
			$fn=strtolower($fn);

			$pp=strpos($fn,'|');
			if($pp)
				$fn=substr($fn,0,$pp);

			$GLOBALS['fxptdata']['lvalues'][$mc][$ln][$fn]=$cv;
//echo('Set empty line for <b>'.$fn.'</b> ($mc='.$mc.', $ln='.$ln.')<hr />');
		}
	}
}

/**
 * Set the global field variables that contain the border information for the mask
 *
 * @param array   $fa - Mandatory parameter:                   Field array containing all field names as values (optional border style can be added to name by a pipe, e.g. 'column|1'
 * @param integer $bs - Optional parameter (default = 0):      Border style (Bit 0-3) - All=15 -> Bit 0=1=top, Bit 1=2=right, Bit 2=4=bottom, Bit 3=8=left
 * @param integer $mn - Optional parameter (default = -1):     Mask number (only used if >= 0)
 * @param integer $mc - Optional parameter (default = -1):     Mask counter (only used if >= 0)
 * @param integer $ln - Optional parameter (default = -1):     Line number (only used if >= 0)
 * @param boolean $el - Optional parameter (default = false):  Insert an empty line before setting the border information
 */
function fxf_maskColumnBorder($fa, $bs=0, $mn=-1, $mc=-1, $ln=-1, $el=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$bs=(int)$bs;
	$mn=(int)$mn;
	$mc=(int)$mc;
	$ln=(int)$ln;

	if(fxIsArray($fa))
	{
		// Insert an empty line?
		if($el)
			fxf_maskEmptyLine($fa, $mc, $ln);

		foreach($fa as $fn)
		{
			$b=$bs;
			$pp=strpos($fn,'|');
			if($pp)
			{
				$b=(int)substr($fn,$pp+1);
				$fn=substr($fn,0,$pp);
			}

			if($mn >= 0)
				$fn .= $mn;
			if($mc >= 0)
				$fn .= '__'.$mc;
			if($ln >= 0)
				$fn .= '_'.$ln;

			if($el === 0)
			{
				if(isset($GLOBALS[$fn.'_ffs']))
					unset($GLOBALS[$fn.'_ffs']);
			}
			else
				$GLOBALS[$fn.'_ffs']=$b;
//echo('Set <b>'.$fn.'</b> to '.$b.'<hr />');
		}
	}
}

/**
 * Create a section header for lists, i.e. setting the values for $GLOBALS['fxptdata']['lvalues']
 *
 * @param integer $mc    - Mandatory parameter:                Mask counter
 * @param integer $l     - Mandatory parameter:                Line number
 * @param string  $txt   - Mandatory parameter:                Header text
 * @param string  $scl   - Mandatory parameter:                Start column name
 * @param string  $ecl   - Optional parameter (default = ''):  End column name
 * @param integer $cs    - Optional parameter (default = 1):   Colspan
 * @param various $color - Optional parameter (default = -2):  Text color value, either an integer or a specific 6-digit hex html color like 'ff0000' for red
 * @param integer $bd    - Optional parameter (default = 0):   Border
 * @param string  $bgc   - Optional parameter (default = ''):  6-digit hex html background color like '0000ff' for blue
 * @param string  $cadd  - Optional parameter (default = ''):  Additional column style attributes
 */
function fxf_listSectHeader($mc, $l, $txt, $scl, $ecl='', $cs=1, $color=-2, $bd=0, $bgc='', $cadd='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Endspalte
	if(!$ecl || ($cs < 1))
		$cs=1;

	$color=trim($color);
	$icolor=(int)$color;
	$pcolor=$icolor;
	if((string)$icolor !== $color)
		$icolor=-2;

	$lwe='^class_';
	if($icolor >= -2)
	{
		$lhc='';
		if(($color === 'r') || ($color === 'g') || ($color === 'b'))
			$lhc=$color;
		$lwe .= 'l'.$lhc.'header';
	}
	else
		$lwe .= 'transparent';

	// Farbe
	$fcol='';
	if(strlen($color) == 6)
		$fcol='color:#'.$color.';';
	$bcol='';
	if(strlen($bgc))
	{
		if(strlen($bgc) == 6)
			$bgc='#'.$bgc;
		$bcol='background:'.$bgc.';';
	}

	if($icolor >= 0)
	{
		$icolor=$icolor%14;
		$carr=array('black','darkgrey','grey','lightgrey','white', 'darkred','red','lightred', 'darkgreen','green','lightgreen', 'darkblue','blue','lightblue');
		$lwe .= ' '.$carr[$icolor];
	}
	else if(($icolor == -1) || (($icolor <= -3) && ($icolor > -99)))
		$lwe .= ' rv'.(($l%2)+1);
	else if(!$pcolor && strlen($color))
		$lwe .= ' '.$color;
	$lwe .= '" nowrap';
	if($cs > 1)
	{
		$ehz='"';
		$lwe .= ' colspan="'.(int)$cs.'"';
	}
	else
		$ehz='';

	if(strlen($fcol) || strlen($bcol) || strlen($cadd))
		$lwe .= ' style="'.$fcol.$bcol.$cadd.$ehz;

	$scl=strtolower($scl);
	if($cs > 1)
	{
		$ecl=strtolower($ecl);
		$GLOBALS['fxptdata']['lvalues'][$mc][$l][$scl]=$lwe.'><!--|~'.$txt;
		$GLOBALS['fxptdata']['lvalues'][$mc][$l][$ecl]='~-->'.$txt;
	}
	else
		$GLOBALS['fxptdata']['lvalues'][$mc][$l][$scl]=$lwe.'|~'.$txt;
}
?>