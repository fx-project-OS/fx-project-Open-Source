<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : d_manager.inc                                                //
// Version     : 21.1                                                         //
// Begin       : 2020-11-18                                                   //
// Last Change : 2021-03-08                                                   //
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
 * AJAX: File Manager
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
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
		$__bsl=true;
		require($__prg);
	}
	else
		die('<div style="padding:8px;"><h1 style="padding:8px;color:red;background:#fff;border:2px solid red;box-shadow:4px 4px 4px rgba(0,0,0, 0.5);">fx-project - ERROR: Necessary file &quot;'.$__pnm.'&quot; not found!</h1></div>');
}

// Can we create PDF...
$GLOBALS['_ext_pdf']='';
//$GLOBALS['_ext_pdf_tcpdf']=true;
$GLOBALS['_ext_pdf_pst']=false;
$req_fn=fxf_fn_reqFilename('ext_pdf');
if(strlen($req_fn))
	require($req_fn);

// ...or SPREADSHEET?
$GLOBALS['_ext_spreadsheet']='';
/*
$req_fn=fxf_fn_reqFilename('ext_spreadsheet');
if(strlen($req_fn))
	require($req_fn);
*/

// Variablen
$rd_debug=0;
$pdf_debug_methodcalls=0;
$pdf_debug_converter=0;
$pdf_get_col_width_debug=0;
$pdf_get_zinfo_debug=0;
$pdf_grafik_debug=0;
$pdf_seite_debug=0;		// Spaltenberechnung anzeigen
$load_pdf_config_debug=0;
$extract_text_debug=0;

// Wertidentifizierer für Download
$ident='"';

if(isset($_preview_config))
	ob_start();
else
{
	setForm(true);
	fxSession('t','l');
	if(fxIsArray($_GET))
		extract($_GET);
}
//fxDebug($GLOBALS['fxptdata'],'fxptdata', 0);
//fxDebug($GLOBALS['fxptdata']['lvalues'],'fxptdata[lvalues]', 0);

// Download-File generieren
$_backup_tr=$GLOBALS['fxpglobals']['tr'];
$_backup_lang=$GLOBALS['fxpglobals']['lang'];
$tr=(int)$_GET['tr'];
if(isset($_preview_tr))
{
	$tr=$_preview_tr;
	$lang=$lng;
}
else
	$lang=$_backup_lang;
//echo('$tr='.$tr.'<br />');
$GLOBALS['fxpglobals']['tr']=$tr;
$GLOBALS['fxpglobals']['lang']=$lang;

if($rd_debug)
	fxDebug($_GET, '$_GET -- Zeile '.__LINE__);

// Prüfen, ob Zusatzinfos mitgeschickt wurden
if(strlen($_GET['zusatz']))
{
//fxDebug($_GET['zusatz'], 'zusatz');
	$zusarr=explode('|', substr($_GET['zusatz'], 1));
	if(is_array($zusarr))
	{
		foreach($zusarr as $zusvar)
			${$zusvar}=true;
	}
}

// Style wegen Farbinfos einladen
fxpGetCSSColors($pdf_allwhite);

define('REPORTDOWNLOADTRENNZEICHEN', ';');	// Semikolon

// Temporary filename
$ad=$GLOBALS['fxpglobals']['client'].'-'.$GLOBALS['fxpglobals']['user'].'-'.$GLOBALS['lts'].'-'.date('YmdHis').'-001';

// Einstellungswerte für Druckaufbereitung verwenden
$settings_backup=$GLOBALS['fxpglobals']['settings'];
$GLOBALS['fxpglobals']['settings']['formatdatum']		= $GLOBALS['fxpglobals']['settings']['d_formatdatum'];
$GLOBALS['fxpglobals']['settings']['formatzeit']		= $GLOBALS['fxpglobals']['settings']['d_formatzeit'];
$GLOBALS['fxpglobals']['settings']['deztrennzeichen']	= $GLOBALS['fxpglobals']['settings']['d_deztrennzeich'];
$GLOBALS['fxpglobals']['settings']['nkstellen']			= $GLOBALS['fxpglobals']['settings']['d_nkstellen'];

// Währung/Einheiten
$units=getUnits();
if($pdf_nocurrency)
	$waehrung='';

// Zeilenumbruchersetzungen
$nlrep="[nl]";
$nllen=-strlen($nlrep);

// Arrays löschen
$strrep=array('&quot;'=>'"', '&nbsp;'=>chr(32), '&nbsp'=>chr(32), '&apos;'=>"'", '&delta;'=>'ø', '&Delta;'=>'ø', '&sum;'=>'+', '&middot;'=>'-', '&uarr;'=>'^', '&darr;'=>'¦', '&larr;'=>'«', '&rarr;'=>'»', '&amp;'=>'&');
// CSV
$GLOBALS['csv_array']=array('file'=>$GLOBALS['tmppath'].'~'.$ad.'.csv', 'lcol'=>0, 'headlines'=>array(), 'headers'=>array(), 'units'=>array(), 'lines'=>array());
$print_array=array();

$he=get_html_translation_table(HTML_ENTITIES, ENT_QUOTES, FXP_CHARSET);
//fxDebug($he,'$he');
$trans=array_flip($he);
$trans['&nbsp;']=" ";
$trans['&apos;']="'";
$trans['&delta;']='ø';
$trans['&Delta;']='ø';
$trans['&sum;']='+';
$trans['&quot;']='"';
$trans['&middot;']='-';
$trans['&uarr;']='^';
$trans['&darr;']='¦';
$trans['&larr;']='«';
$trans['&rarr;']='»';
$trans['&amp;']='&';
//fxDebug($trans,'$trans');

$ptrans=array(chr(13).chr(10)=>'<br />', chr(10).chr(13)=>'<br />', chr(13)=>'<br />', chr(10)=>'<br />', '<br>'=>'<br />', '&nbsp;'=>chr(32), '&apos;'=>"'", '&delta;'=>'ø', '&Delta;'=>'ø', '&sum;'=>'+', '&quot;'=>'"', '&middot;'=>'-', '&uarr;'=>'^', '&darr;'=>'¦', '&larr;'=>'«', '&rarr;'=>'»', '&lt;'=>'<', '&gt;'=>'>', '&amp;'=>'&');

// Maskenfelder rückholen?
if(isset($_preview_mask))
{
	$GLOBALS['_masknumber']=$_preview_mask;
	sessionMask($GLOBALS['_masknumber']);
	if(isset($_preview_madd) && is_array($_preview_madd))
		$GLOBALS['fxptdata']['mfa_'.$GLOBALS['_masknumber']]=$_preview_madd;
}
else if($GLOBALS['_masknumber'])
	sessionMask($GLOBALS['_masknumber']);
//fxDebug($GLOBALS['fx_mask'][$GLOBALS['_masknumber']], $GLOBALS['_masknumber'], 0);
//fxDebug($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']], '$GLOBALS[\'fx_mfields\']['.$GLOBALS['_masknumber'].']', 0);
//fxDebug($GLOBALS['fx_mheaders'][$GLOBALS['_masknumber']], '$GLOBALS[\'fx_mheaders\']['.$GLOBALS['_masknumber'].']', 0);

// Headline ermitteln
$mheadline=trim($GLOBALS['fxptdata']['headline'.$GLOBALS['_masknumber']]);
if(strlen($mheadline))
	$mheadline=trim(maskHeadlineEntries($mheadline, false));
$print_array[-3]=strtr(strip_tags($mheadline), $strrep);
$GLOBALS['csv_array']['headlines'][0]=trim(strip_tags(strtr($print_array[-3], $trans)));
if($headline !== 'blank')
{
	if(substr($headline, 0, 1) == '+')
		$headline=trim($GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['text_kurz'].substr($headline, 1));
	if(!strlen($headline))
		$headline=trim($GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['text_kurz']);
	if(!strlen($headline))
		$headline=trim($print_array[-3]);
	else
		$headline=trim(maskHeadlineEntries($headline, false));
	$headline=strtr($headline, $strrep);

	$GLOBALS['csv_array']['headlines'][1]=trim(strip_tags(strtr($headline, $trans)));
	$print_array[-2]=$headline;
}
else
{
	$GLOBALS['csv_array']['headlines'][1]='Report';
	$print_array[-2]='Report';
}

if(isset($_preview_headline))
{
	$print_array[-3]=$_preview_headline;
	$print_array[-2]=$_preview_headline;
}
else if($tr == 191)
	$print_array[-2]=meldung(1498,false);	// Reisekosten
//fxDebug($print_array, '$print_array', 0);

// Feldwerte einlesen
$feldwerte_download=array();
if(isset($_preview_values) && fxIsArray($_preview_values))
	$feldwerte_download=$_preview_values;
else if($_GET['msk_counter'])
	$feldwerte_download=$GLOBALS['fxptdata']['lvalues'][$_GET['msk_counter']];
else if($listensql)
	$feldwerte_download=@db_values(urldecode($listensql), '*');
elseif($_GET['tr'] == 84)
	$feldwerte_download=$GLOBALS['fxptdata']['backup']['psp_n'];
//fxDebug($_GET['msk_counter'], '$_GET[\'msk_counter\']', 0);
//fxDebug($feldwerte_download, '$feldwerte_download', 0);

if(!fxIsArray($feldwerte_download))
{
	dm_window('');
	die;
}

if($rd_debug == 2)
	fxDebug($feldwerte_download, '$feldwerte_download -- Zeile '.__LINE__);

$mfa=$GLOBALS['fxptdata']['mfa_'.$GLOBALS['_masknumber']];
//fxDebug($mfa, '$mfa', 0);

// Überschriften ermitteln
$sex=explode(',', $mfa['columns']);
$sanz=0;
$ssize=sizeof($sex);
while($sanz < $ssize)
{
	if(strpos($sex[$sanz], '*') > 0)
	{
		$sanz++;
		continue;
	}
	$column=(int) $sex[$sanz];

	if($mfa[1][$column] === 'skip')
		break;

	$gex=explode(',', $mfa[1][$column]);
	$gcnt=0;
	$gsize=sizeof($gex);
	while($gcnt < $gsize)
	{
		$g=(int)$gex[$gcnt];
		if(isset($_preview_header) && is_array($_preview_header) && $_preview_header[$g])
		{
			$columnheader=$_preview_header[$g];
//echo('Preview-'.$g.': '.$columnheader.'<br />');
		}
		else if(isset($GLOBALS['fx_mheaders'][$GLOBALS['_masknumber']][$g]))
		{
			$columnheader=$GLOBALS['fx_mheaders'][$GLOBALS['_masknumber']][$g];
//echo('Header-'.$g.': '.$columnheader.'<br />');
		}
		else
		{
			$showlit='';
			$mfld=$anzart_defines[(int) $GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['anzeigeart']];
			if(substr($mfld, 0, 7) != 'literal')
				$showlit=4;
			$columnheader=get_array_entry($GLOBALS['_masknumber'], '', $GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g], false, '', $GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['status_maske'], $showlit);
//echo('Showlit-'.$showlit.': '.$columnheader.'<br />');
		}
		$columnheader=strtr(strip_tags($columnheader), $strrep);
		$dl_columnheader=$columnheader;

		if(substr(strtolower($columnheader), 0, 5) != 'dummy')
		{
			if(strlen($waehrung) && strlen($dl_columnheader) && (($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldtyp'] == 11) || ($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldtyp'] == 16) || ($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldtyp'] == 17)) && ((substr($mfld, 0, 7) != 'literal'))) // Waehrung
				$dl_columnheader .= $ident.REPORTDOWNLOADTRENNZEICHEN.$ident;

			// PDF
			$print_array[-1][$g]=trim(strtr($columnheader,$trans));

			// CSV
			$GLOBALS['csv_array']['headers'][$g]=trim(strip_tags(strtr($columnheader,$trans)));
			$GLOBALS['csv_array']['lcol']=max($GLOBALS['csv_array']['lcol'],$g);
		}
		$gcnt++;
	}
	$sanz++;
}

if($pdf_noheadlines)
{
	$GLOBALS['csv_array']['headers']=array();
	$GLOBALS['csv_array']['lcol']=0;
}

$refw=$GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['refw'];
// Body generieren
$lca=array();
$elica=array('icsh.png', 'icne.png', 'icch.png', 'icde.png');
$zcounter=0;
foreach($feldwerte_download as $aktzeile => $fwd)
{
	$sex=explode(',', $mfa['columns']);
	$sanz=0;
	$ssize=sizeof($sex);
	while($sanz < $ssize)
	{
		if(strpos($sex[$sanz], '*') > 0)
		{
			$sanz++;
			continue;
		}
		$column=(int) $sex[$sanz];
		if($mfa[1][$column] === 'skip')
			break;
		if(($mfa[1][$column] === 'blank') && !$maskenliste && ($GLOBALS['fx_mask'][$GLOBALS['_masknumber']]['listenansicht'] != 1))
		{
			$sanz++;
			continue;
		}

		$gex=explode(',', $mfa[1][$column]);
		$g=(int) $gex[0];
		$mfld=$anzart_defines[(int) $GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['anzeigeart']];
		$fldnm=strtolower($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname']);

		if(substr($fldnm, 0, 5) != 'dummy')
		{
			$tdwaehrung='';
			$lower_feldname=strtolower($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname']);
			if(!$GLOBALS['no_waehrung'] && (substr($mfld,0,7) != 'literal') && ($mfld != 'bild') && ($mfld != 'icon') && ($mfld != 'checkbox') && (substr($lower_feldname,0,5) != 'dummy')) // Währung nur falls kein Literal, kein Dummyfeld und nicht ausgeschaltet
			{
				// Betragsfeld (in Firmenwährung)
				if(($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldtyp'] == 11) || ($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldtyp'] == 16) || ($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldtyp'] == 17))
					$tdwaehrung=$waehrung;
				// Aufwandsfeld in Reports (in Stunden)
				else if((strtoupper(substr($GLOBALS['_masknumber'],0,1)) == 'R') && (substr($lower_feldname,0,7) == 'aufwand'))
					$tdwaehrung='h';
			}
			// Feldwert ermitteln
			$fw_pre		= '';
			$feldwert	= trim($fwd[strtolower($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname'])]);
//fxDebug($feldwert, $aktzeile.': ['.$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname'].']');
			if($rd_debug)
			{
				if(!$sanz)
					echo('<HR SIZE="1" NOSHADE><B><U><FONT COLOR="#880000">'.$zcounter.':</FONT></U></B><BR>');
				echo($g.': <B>'.$fldnm.'</B> -- $feldwert="<FONT COLOR="#000088">'.fxHtmlEncode($feldwert).'</FONT>"<BR>');
			}
			if($GLOBALS['fxptdata']['nounits_'.$GLOBALS['_masknumber']][$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname']] || (strlen($tdwaehrung) && strlen($feldwert) && (strpos($feldwert, $tdwaehrung) !== false)))
				$tdwaehrung='';
//fxDebug($GLOBALS['fxptdata']['nounits_'.$GLOBALS['_masknumber']][$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname']], $GLOBALS['_masknumber'].' ('.$g.'): ['.$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['feldname'].']='.$tdwaehrung);

			// Farben gesondert behandeln
			if((substr($feldwert, 0, 7) == '^class_') || (substr($feldwert, 0, 4) == '^id_') || (substr($feldwert, 0, 5) == '^col_')) // Farbangaben eliminieren
			{
				$pipepos=strpos($feldwert, '|');
				if($pipepos)
				{
					$fw_pre		= substr($feldwert, 0, $pipepos);
					$feldwert	= substr($feldwert, $pipepos+1);
				}
			}
			if($rd_debug)
				echo('<b>$feldwert:</b><pre style="border:1px solid grey;border-radius:4px;padding:4px;background:#ffffff;color:#e1001a;white-space:normal;">'.fxHtmlEncode($feldwert).'</pre>');

			// Hidden-Felder eliminieren
			while(strlen($feldwert))
			{
				$hfsp=stripos($feldwert,'<input type="hidden"');
				if($hfsp !== false)
				{
					$hfep=strpos($feldwert,'>',$hfsp);
					if($hfep === false)
						$feldwert=substr($feldwert,0,$hfsp);
					else
					{
						$tpa=1;
						while(true)
						{
							$tsp=strpos($feldwert,'<',$hfsp+$tpa);
							if(($tsp !== false) && ($tsp < $hfep))
							{
								$tpa=$tsp;
								$hfep=strpos($feldwert,'>',$hfsp+$tpa);
							}
							else
								break;
						}
						$feldwert=substr($feldwert,0,$hfsp).substr($feldwert,$hfep+1);
					}
				}
				else
					break;
			}
			// HTML-Kommentare eliminieren
			$htmlcs=strpos($feldwert, '<!--');
			if($htmlcs !== false)
			{
				$htmlce=strpos($feldwert, '-->', $htmlcs);
				if($htmlce === false)
					$feldwert=substr($feldwert, 0, $htmlcs);
				else
					$feldwert=substr($feldwert, 0, $htmlcs).substr($feldwert, $htmlce+3);
			}
			$htmlce=strpos($feldwert, '-->');
			if($htmlce !== false)
			{
				$fw_pre .= '#endkom#';
				$feldwert=substr($feldwert, $htmlce+3);
				$columnentry='';
			}
			else if(substr($feldwert, 0, 1) == '~') // Feld ohne Überprüfung übernehmen
				$columnentry=substr($feldwert, 1);
			else
			{
				$columnentry=get_db_value($fldnm, $feldwert);
				if($columnentry === false)
				{
					if($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['nachschlage'] && ($mfld != 'auswahlfeld')) // Nachschlagewert umwandeln
					{
						if($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['repeval'])
						{
							if($refw[$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['repeval']])
							{
								$retr_table=array('$var' => $feldwert);
								$repeval=strtr($refw[$GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]['repeval']], $retr_table);
								eval($repeval);
								$columnentry=$feldwert;
							}
							else
								$columnentry='<I>'.$feldwert.'</I>';
						}
						else
						{
							if(!$GLOBALS['db_values']['wertetab'][$feldwert])
							{
								$mld=@meldung($feldwert, true);
								if($mld)
									$GLOBALS['db_values']['wertetab'][$feldwert]=get_entry($mld);
								else
									$GLOBALS['db_values']['wertetab'][$feldwert]=$feldwert;
							}
							$columnentry=$GLOBALS['db_values']['wertetab'][$feldwert];
						}
					}
					else // Feld generieren
						$columnentry=get_entry($feldwert, $GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g]);
				}
			}
//fxDebug(fxHtmlEncode($columnentry),'$columnentry');

			// Aktionsicons eliminieren
			foreach($elica as $elic)
			{
				$eip=stripos($columnentry,$elic);
				if($eip !== false)
				{
					$lca[$g]=true;
					$imsp=strrpos($columnentry,'<i',$eip-strlen($columnentry));
					$imep=strpos($columnentry,'">',$eip);
					if(($imsp !== false) && ($imep !== false) && ($imsp < $eip) && ($eip < $imep))
						$columnentry=substr($columnentry,0,$imsp).substr($columnentry,$imep+2);
				}
			}
//fxDebug(fxHtmlEncode($columnentry),'$columnentry');

			while(substr($columnentry,0,6) == '&nbsp;')
				$columnentry=substr($columnentry,6);
			while(substr($columnentry,0,5) == '&nbsp')
				$columnentry=substr($columnentry,5);
			while(substr($columnentry,-6) == '&nbsp;')
				$columnentry=substr($columnentry,0,-6);
			while(substr($columnentry,-5) == '&nbsp')
				$columnentry=substr($columnentry,0,-5);
			$dl_columnentry=str_ireplace('<br />', ' ', $columnentry);
			if($feldwert == '~^')
			{
				$dl_columnentry	= '~';
				$columnentry	= '~';
			}
			else if($feldwert == '~|')
			{
				$dl_columnentry	= '|';
				$columnentry	= '|';
			}
			else
			{
			 	if($feldwert == '')
				{
					$dl_columnentry	= '';
					$columnentry	= '^';
				}
				if($tdwaehrung != '')
				{
					$dl_columnentry	.= $ident.REPORTDOWNLOADTRENNZEICHEN.$ident.$tdwaehrung;
					$columnentry	.= ' '.$tdwaehrung;
				}
			}

			if($rd_debug)
			{
				echo('<b>$columnentry:</b><pre style="border:1px solid grey;border-radius:4px;padding:4px;background:#ffffff;color:#006b9f;white-space:normal;">'.fxHtmlEncode($columnentry).'</pre>');
				echo('<b>$dl_columnentry:</b><pre style="border:1px solid grey;border-radius:4px;padding:4px;background:#ffffff;color:#009f6b;white-space:normal;">'.fxHtmlEncode($dl_columnentry).'</pre>');
			}

			// PDF
			$uca=replaceUnicode(trim(strtr($columnentry,$ptrans)),'u');
			$ucr=$uca['result'];
			// ...Replace timespan?
			$tss=strpos($ucr,'<span id=tshd_');
			if($tss !== false)
			{
				$ses=strpos($ucr,'sec=',$tss+14);
				$see=strpos($ucr,'>',$ses+4);
				$tse=strpos($ucr,'</span>',$see+1);

				$ucr=substr($ucr,0,$tss).get_entry((int)substr($ucr,$ses+4,$see-$ses-4),'zeitspanne').substr($ucr,$tse+7);
			}
			$print_array[$zcounter][$g]['show']=$ucr;
			if($rd_debug)
				echo('<b>$show ('.$g.'):</b><pre style="border:1px solid grey;border-radius:4px;padding:4px;background:#ffffff;color:#000000;">'.fxHtmlEncode($print_array[$zcounter][$g]['show']).'</pre>');
			$print_array[$zcounter][$g]['fw']=$feldwert;
			if($fw_pre)
				$print_array[$zcounter][$g]['fwp']=$fw_pre;

			// CSV
			$ue=-1;
			$fdf=0;
			$fnf=0;
			$ftf=0;
			$fpf=0;
			$faf=0;
			$ctext=trim($columnentry);
			if((substr($ctext,0,1) == '~') || (substr($ctext,0,1) == '^') || (substr($ctext,0,1) == '|'))
				$ctext=trim(substr($ctext,1));
//echo('<pre style="margin:0;margin-top:2px;padding:4px;background:#ffffff;">'.fxHtmlEncode($ctext).'</pre>');
			if(strlen($ctext))
			{
				$ctext=strtr($ctext,$ptrans);
				// ...Line break?
				$lb=false;
				if(stripos($ctext,'<br />') !== false)
				{
//echo('<pre style="margin:0;margin-top:2px;padding:4px;background:#dddddd;">'.fxHtmlEncode($ctext).'</pre>');
					$ctext=str_ireplace('&nbsp;', ' ', $ctext);
					$ctext=str_ireplace('<br />', $nlrep, $ctext);
					while(substr($ctext,$nllen) == $nlrep)
						$ctext=trim(substr($ctext,0,$nllen));
//echo('<pre style="margin:0;padding:4px;background:#eeeeee;">'.fxHtmlEncode($ctext).'</pre>');
				}
				$ctext=trim(strip_tags(strtr($ctext,$trans)));
				while(stripos($ctext,'  ') !== false)
					$ctext=str_ireplace('  ', ' ', $ctext);
				$ctext=trim(str_ireplace($nlrep.' ', $nlrep, $ctext));
//echo('<pre style="margin:0;margin-top:2px;padding:4px;background:#ffffff;">'.fxHtmlEncode($ctext).'</pre>');
				$sctext=strlen($ctext);
				$lb=stripos($ctext,$nlrep);
				// ...Unit entry?
				$uc='';
				foreach($units as $up => $unit)
				{
					$chk=' '.$unit;
					$schk=strlen($chk);
					if(($sctext > $schk) && (substr($ctext, $sctext-$schk) === $chk))
					{
						if($lb !== false)
						{
							$ctext=trim(substr($ctext,0,$lb));
							$sctext=strlen($ctext);
							$lb=false;
						}
						$ctext=trim(substr($ctext,0,$sctext-$schk));
						$ue=$up;
						$uc=$unit;
						break;
					}
				}
				// ...Format?
				if(($lb === false) && strlen($ctext))
				{
					for($t=0; $t<strlen($ctext); $t++)
					{
						$cc=substr($ctext,$t,1);
						if(($cc == ',') || ($cc == '.') || ($cc == '-'))			// ,.-
						{
							$fdf++;
							if(($fdf == 2) && (substr($ctext,$t-4,1) != $cc))
							{
								$faf++;
								$fdf=0;
							}
						}
						else if($cc == '%')							// %
							$fpf++;
						else if((ord($cc) > 47) && (ord($cc) < 58))	// 0-9
							$fnf++;
						else
							$ftf++;
					}
//echo('<div style="margin:0;margin-top:2px;padding:4px;background:#ffffff;"><b>'.fxHtmlEncode($ctext).'</b><br />$lb='.$lb.' -- $ue='.$ue.', $fdf='.$fdf.', $fnf='.$fnf.', $ftf='.$ftf.', $fpf='.$fpf.', $faf='.$faf.'</div>');
				}
				// Line break? only for texts allowed!
				if($ftf && ($ue >= 0))
				{
					$ue=-1;
					$ctext .= ' '.$unit;
				}
				if($ue >= 0)
					$ctext=get_double($ctext);
				else if(($lb !== false) || $ftf || $faf || (!$fdf && !$fnf))
					$ctext=(string)$ctext;
				else
				{
					if($fdf)
						$ctext=get_double($ctext);
					else
						$ctext=(int)$ctext;
					if($fpf)
						$uc='%';
				}

				if(strlen($uc) && !isset($GLOBALS['csv_array']['units'][$g]))
					$GLOBALS['csv_array']['units'][$g]=$uc;
				if(!isset($GLOBALS['csv_array']['lines'][$aktzeile]))
					$GLOBALS['csv_array']['lines'][$aktzeile]=array();
				$GLOBALS['csv_array']['lines'][$aktzeile][$g]=$ctext;
				$GLOBALS['csv_array']['lcol']=max($GLOBALS['csv_array']['lcol'],$g);
			}

			if(($rd_debug == 2) && !$aktzeile)
				fxDebug($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$g], 'Spalte '.$g.' -- Zeile '.__LINE__);
		}
		$sanz++;
	}
	$zcounter++;
}
//fxDebug($print_array, '$print_array', 0);

// Leere Spalten eliminieren
$eca=array();
if(fxIsArray($print_array))
{
	foreach($print_array as $pal => $pla)
	{
		if(($pal >= 0) && fxIsArray($pla))
		{
			foreach($pla as $pac => $pca)
			{
				$ea=0;
				if(strlen($pca['show']) && ($pca['show'] != '~') && ($pca['show'] != '^') && ($pca['show'] != '|'))
					$ea=1;
				if(!isset($eca[$pac]))
					$eca[$pac]=$ea;
				else
					$eca[$pac] += $ea;
			}
		}
	}
}
//fxDebug($eca,'$eca');
//fxDebug($lca,'$lca');
if(sizeof($eca))
{
	foreach($eca as $ec => $ee)
	{
		if(!$ee)
		{
			$delcol=true;
			// Nur "Link"-Spalten löschen
			if(!isset($lca[$ec]))
				$delcol=false;
			if($delcol)
			{
				// Überschrift löschen
				if(fxIsArray($print_array[-1]) && isset($print_array[-1][$ec]))
					unset($print_array[-1][$ec]);
				// Spalte löschen
				foreach($print_array as $pal => $pla)
				{
					if(($pal >= 0) && fxIsArray($pla) && fxIsArray($pla[$ec]))
						unset($print_array[$pal][$ec]);
				}
			}
		}
	}
}
//fxDebug($print_array, '$print_array', 0);

// Rechnungs-ID ermitteln, falls übermittelt wurde
$rechnung_array=array();
$rechnung_pos_array=array();
$rechid=0;
if(isset($_preview_invid))
	$rechid=$_preview_invid;
if(!$rechid && isset($_GET) && isset($_GET['Rechnungs_ID']))
	$rechid=(int)$_GET['Rechnungs_ID'];
//echo('<b>$rechid</b>='.$rechid.'<br />');

$pdf_data='';
$pdfloaded=false;

$config=0;
$selname='';
if((int)$GLOBALS['_masknumber'] == 294)
	$config=1;
else if($rechid)
{
	$config=2;

	if(isset($_preview_invoice) && is_array($_preview_invoice) && sizeof($_preview_invoice))
	{
		$rechnung_array=$_preview_invoice;
		$selname=trim($rechnung_array['rechnungsform']);
	}
	else
	{
		$rsql="SELECT * FROM rechnung WHERE rechnungs_id=".$rechid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$rechnung_array=db_values($rsql);
//fxDebug($rechnung_array,$rsql, 0);
		if(fxIsArray($rechnung_array))
		{
			$rsql="SELECT positions_id, mwst_satz, mwst, projekt_id, artikelzuord_id, buendelpos FROM rechnungsposition WHERE rechnungs_id=".$rechid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY mwst_satz";
			$rechnung_pos_array=db_values($rsql, '*');
//fxDebug($rechnung_pos_array,$rsql, 0);

			$selname=trim($rechnung_array['rechnungsform']);

			if(!$rechnung_array['ausgegangen_am'])
			{
				// Im Debugmodus das Ausgangsdatum in der Rechnung nicht setzen
				if(!$GLOBALS['__DGBLVL'])
				{
					$jetzt=fxNow();
					$rechnung_array['ausgegangen_am']=$jetzt;
					$rechnung_array['zeitstempel']=$jetzt;

					$sql="UPDATE rechnung SET ausgegangen_am='".$jetzt."' WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND rechnungs_id=".$rechid;
					@db_x($sql);
				}
			}
			else if(!$_GET['pdfs_button'])
			{
				$dsql  = "SELECT v.pfad, v.zeitstempel AS vzs FROM d_dokumente d, d_dok_verlauf v";
				$dsql .= " WHERE d.elter_art=8 AND d.dok_art=2580 AND d.id=".$rechid." AND d.mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$dsql .= " AND v.dok_id=d.dok_id AND v.aktiv_kz=1 AND v.mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$dpza=db_values($dsql);
//fxDebug($dpza, $dsql);
				if(isArray($dpza) && strlen($dpza['pfad']) && ($dpza['vzs'] >= $rechnung_array['zeitstempel']))
				{
					$dpfad=$dpza['pfad'];
					if(!isset($GLOBALS['doc']))
					{
						if(!class_exists('fxpDoc'))
						{
							$GLOBALS['__includer']['dms']=true;
							require('includer.inc');
						}
						if(class_exists('fxpDoc'))
							$GLOBALS['doc']=new fxpDoc();
					}
					if(isset($GLOBALS['doc']))
					{
						$dok_fn=$GLOBALS['doc']->cpath.$dpfad;
//fxDebug($dok_fn,'$dok_fn');
						$pdf_data=fxLoadBin($dok_fn);
						if($pdf_data)
							$pdfloaded=true;
					}
				}
			}
		}
	}
//fxDebug($rechnung_array, $rsql);
}
else if(((int)$GLOBALS['_masknumber'] == 2) || ((int)$GLOBALS['_masknumber'] == 231))
	$config=3;

// Skalierungsfaktor für Grafiken
$pdf_picscale=0.65;
// "Luft" am unteren Rand (wird später genauer errechnet)
$pdf_space=0.00;

// Konfigurationsfile einladen und parsen
$pdf_array=load_pdfconfig($config, $selname);
//fxDebug($pdf_array, '$pdf_array', 0);

// Zusatzinfos zum Array
if($config != 2)
	$pdf_array['filename']=$print_array[-2];	// Filename der zu erzeugenden PDF-Datei
else
	$pdf_array['filename']=meldung(872, false).'_'.$rechnung_array['rechnn_f'];
$pdf_array['i_title']=$print_array[-3];		// Titel für das Info-Feld
$pdf_array['i_subject']=$print_array[-2];	// Subject für das Info-Feld

$pdf_array['min_y']=$pdf_array['tmargin'];	// Obere Begrenzung
$pdf_array['max_y']=$pdf_array['bmargin'];	// Untere Begrenzung
$pdf_array['h_pfoot']=(float)16.0;	// Höhe der FeRox-Fusszeile
$pdf_array['w_body']=$pdf_array['width']-$pdf_array['lmargin']-$pdf_array['rmargin'];	// Innere Breite

if(isset($pdf_array['f_nofoot']))
	$pdf_array['h_pfoot']=(float)0.0;

// Ein neues PDF-Objekt erstellen mit Standard-Fehlerhandling und -Speicherverwaltung.
$pdf=false;
if(strlen($GLOBALS['_ext_pdf']) && (!$pdf_data || $_GET['forcecreate']))
	$pdf=fxpdf_new();
//fxDebug($pdf,'$pdf', 0);

if($pdf)
{
	// PDF-Dokument öffnen und Infofelder setzen
	fxpdf_page($pdf, 'open');
	// Fusszeileninfo ermitteln
	$dmb=fxpdf_foot_info();

	$subseite	= 0;		// Subseite
	$sts		= 0;		// Startspalte
	$mxs		= 1;		// max. Spaltenanzahl	(wird später genauer bestimmt)
	$xmax		= 0.00;		// max. X-Koordinate
	$summe		= array();	// Übertrag bzw. Gesamtsumme
	$sumset		= array();
	$ria		= array();	// Rechnungskopf
	$hset		= false;	// Header bereits geschrieben
	$lto		= 1;		// Multizeilenpointer

	// Alle Spalten durchgehen
	while($sts < $mxs)
	{
//echo(__LINE__.': while($sts='.$sts.' < $mxs='.$mxs.'))<br />');
		$seite	= 1;		// Startseite
		$stz	= 0;		// Startzeile
		if(!$mxz)
			$mxz=1;		// max. Zeilenanzahl	(wird später genauer bestimmt)
		$nxt=true;
		$nsts=$mxs;

		// Alle Zeilen durchgehen
		while($nxt || ($stz < $mxz))
		{
//echo(__LINE__.': while($nxt='.$nxt.' || ($stz='.$stz.' < $mxz='.$mxz.'))<br />');
			$nxmax='';
			// Neue Seite im vordefinierten Format erstellen.
			fxpdf_page($pdf, 'begin');

			if(!$pdf_array['fs'])
			{
				// Zeichensatzgrösse, Spaltenbreite, Zeilenanzahl + Seitenanzahl
				$pdf_lines		= array();
				$pdf_col_width	= array();
				$pdf_columns	= array();
				$pdf_tooltips	= array();
				$pdf_subwidth	= 0.0;
				$pdf_subheight	= 0.0;
				$pdf_subwidth2	= 0.0;
				$pdf_subheight2	= 0.0;

				dpdf_get_zinfo($pdf);
//fxDebug($pdf_array, '$pdf_array -- Zeile '.__LINE__);
				if($rd_debug)
				{
					fxDebug($GLOBALS['csv_array'], '$csv_array -- Zeile '.__LINE__);
					fxDebug($print_array, '$print_array -- Zeile '.__LINE__);
				}
				if($pdf_grafik_debug)
				{
					fxDebug($pdf_columns, '$pdf_columns -- Zeile '.__LINE__);
					fxDebug($pdf_col_width, 'Spaltenbreiten-Array $pdf_col_width -- Zeile '.__LINE__);
				}
//fxDebug($pdf_col_width, 'Spaltenbreiten-Array $pdf_col_width -- Zeile '.__LINE__);
//fxDebug($pdf_lines, 'Zeilen-Array $pdf_lines -- Zeile '.__LINE__);
//fxDebug($pdf_lineheight, 'Höhen-Array $pdf_lineheight -- Zeile '.__LINE__);

				$mxs=sizeof($pdf_col_width);	// Max. Spalten
				$mxz=sizeof($pdf_lines);		// Max. Zeilen

				// Gesamtbreite aller Spalten ermitteln
				$colw=0.00;
				$rspalte=array();
				$spc=0;
				foreach($print_array[-1] as $spalte => $value)
				{
					$colw += $pdf_col_width[$spalte];
					$rspalte[$spc++]=$spalte;
				}
//fxDebug($rspalte, '$rspalte -- Zeile '.__LINE__);
//echo('<HR>'.__LINE__.' -- <B>Gesamtbreite aller Spalten:</B> '.$colw.'<BR><BR>');

				// "Luft" am unteren Rand
				$pdf_space=16.0*$pdf_array['fac'];
			}

			// Kopf
			if(!$hset && !$stz && $pdf_array['h_head'])
			{
				fxpdf_clipping($pdf, 1, true, false);
				if($pdf_array['head'])
				{
					$pdf_array['head']=dpdf_user_substitute($pdf, $pdf_array['head'], $print_array[-2]);
					fxpdf_user($pdf, $pdf_array['head']);
				}
			}

			// Body
			$xc1=dpdf_get_start_x($sts, $xmax);
			$yc1=0.0;
			if(!$hset && !$stz)
			{
				fxpdf_clipping($pdf, 0, true, false);
				$yc1=dpdf_chead($pdf, 1, $yc1);	// Rechnungskopf
//fxpdf_rec($pdf, 0.0, 0.0, $pdf_array['w_body'], $pdf_array['h_chead']);
				$hset=true;
			}
			else
			{
				fxpdf_clipping($pdf, 0, false, false);
				if(is_array($GLOBALS['rtv']) && sizeof($GLOBALS['rtv']))
				{
					foreach($GLOBALS['rtv'] as $lnr => $txt)
					{
						if($yc1 < $pdf_array['h_body'])
						{
							fxpdf_text($pdf, $txt, $pdf_array['xd'], $yc1, $pdf_array['fs'], 'left', 'bold');
							unset($GLOBALS['rtv'][$lnr]);
							$yc1 += $pdf_array['fs'];
						}
						else
							break;
					}
				}
				if(is_array($GLOBALS['rta']) && sizeof($GLOBALS['rta']))
				{
					foreach($GLOBALS['rta'] as $lnr => $txt)
					{
						if($yc1 < $pdf_array['h_body'])
						{
							fxpdf_text($pdf, $txt, $pdf_array['xd'], $yc1, $pdf_array['fs']);
							unset($GLOBALS['rta'][$lnr]);
							$yc1 += $pdf_array['fs'];
						}
						else
							break;
					}
				}
			}

			if($yc1 < $pdf_array['h_body'])
			{
				// Gesamtüberschrift ausser bei Rechnung
				if(($config < 1) || ($config > 3))
				{
					$tfs=$GLOBALS['pdf_array']['fshl']*1.5;

					$yc2=$yc1+$tfs*2;
					$xc1=-$xmax;
					$xc2=$xc1+$colw;
					$mwd=$xc2-$xc1;

					$hdw=fxpdf_wraptext($pdf, $print_array[-3], -$tfs, 'B', $mwd);
//fxDebug($hdw, $print_array[-3]);
					if(fxIsArray($hdw))
					{
						$ych=$yc1+$tfs;
						foreach($hdw as $hnr => $hline)
						{
							fxpdf_text($pdf, $hline, '!'.($xc1+$pdf_array['xd']), $ych, -$tfs, 'left', 'bold');
							if($hnr < (sizeof($hdw)-1))
							{
								$ych += $tfs;
								$yc2 += $tfs;
							}
						}
					}
				}
				else
					$yc2=$yc1;

				// Schattenkoordinaten merken
				$syc1=$yc2;

				// Überschrift (wird nur angezeigt, falls $pdf_noheadlines nicht gesetzt ist)
				$xc1=dpdf_get_start_x($sts, $xmax);
				$yc1=$yc2;
				if(!$pdf_noheadlines)
					$yc2=$yc1+$pdf_array['yd']*3.0+$pdf_array['fsh'];
				else
					$yc2=$yc1+$pdf_array['yd'];
				$spalte=$sts;
				if($pdf_seite_debug)
					echo('<HR>'.__LINE__.': $seite='.$seite.', $subseite='.$subseite.' -- $spalte='.$spalte.', $mxs='.$mxs.'<BR>');
				while(($spalte < $mxs) && !$nxmax)
				{
//echo(__LINE__.': while(($spalte='.$spalte.' < $mxs='.$mxs.') && !$nxmax='.$nxmax.')<br />');
					$rsp=$rspalte[$spalte];
					$xc2=$xc1+$pdf_col_width[$rsp];
					// Prüfen, ob über Seitenrand hinausgezeichnet wird
					if($pdf_seite_debug)
						echo('&nbsp;&nbsp;&nbsp;'.__LINE__.': $spalte='.$spalte.'('.$rsp.'), $xc2='.$xc2.' > $w_body='.$pdf_array['w_body'].'<BR>');
					if($xc2 > $pdf_array['w_body'])
					{
						$nsts=$spalte;
						if(!$pdf_array['sep'] || ($nsts == $sts))
						{
							$sxc1	= -9999;
							$nxmax	= $xmax+$pdf_array['w_body'];
						}
						else
						{
							$sxc1	= $xc1;
							$nxmax	= $xmax+$xc1;
						}
						if($pdf_seite_debug)
							echo('&nbsp;&nbsp;&nbsp;<B>'.__LINE__.': $nsts='.$nsts.', $nxmax='.$nxmax.'</B><BR>');
					}
					if(!$pdf_noheadlines)
 						fxpdf_box($pdf, '!'.$xc1, $yc1, '!'.$xc2, $yc2, 'eeeeee', '#', 0.0, 0.0, 0.35, '888888');
					$align=strtolower($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$rsp]['horausr']);
					if($align == 'right')
						$xc=$xc2-$pdf_array['xd'];
					else if($align == 'center')
						$xc=($xc1+$xc2)/2.0;
					else
						$xc=$xc1+$pdf_array['xd'];
					if(!$pdf_noheadlines)
						fxpdf_text($pdf, $print_array[-1][$rsp], '!'.$xc, (($yc1+$yc2)/2.0)+($pdf_array['fsh']/2.5), $pdf_array['fsh'], $align, 'bold', 0.25, '444444');
					$xc1=$xc2;

					$spalte++;
				}

				// Übertrag
				if($stz < $mxz)
					dpdf_sum($pdf, 0);	// Übertrag

				// Inhalt
				$ostz=$stz;
				$az=$stz;
				$wexit=false;
				while(!$wexit && ($az < $mxz))
				{
//echo(__LINE__.': while(!$wexit='.$wexit.' && ($az='.$az.' < $mxz='.$mxz.'))<br />');
					// Leerzeile ohne Einträge?
					$eline=0;
					if(fxIsArray($pdf_columns) && fxIsArray($pdf_columns[$az]))
					{
						$eline=1;
						// Array mit notwendigen Infos (Colspan + Text) erstellen und korrekt sortieren
						$oela=array();
						foreach($pdf_columns[$az] as $cc => $pca)
						{
							$oela[$cc]=array('c'=>$cc, 'cs'=>1, 'txt'=>'');
							if(fxIsArray($pca) && fxIsArray($pca[1]) && fxIsArray($pca[1][0]))
							{
								if(strlen($pca[1][0]['txt']) && ($pca[1][0]['txt'] != '^'))
									$oela[$cc]['txt']=$pca[1][0]['txt'];
								if(isset($pca[1][0]['cs']) && ($pca[1][0]['cs'] > 1))
									$oela[$cc]['cs']=$pca[1][0]['cs'];
							}
						}
						ksort($oela);
//fxDebug($oela,'$oela', 0);

						$nela=array();
						foreach($oela as $oel)
							$nela[]=$oel;
//fxDebug($nela,'$nela', 0);

						$ec=0;
						while(fxIsArray($nela[$ec]))
						{
							if(strlen($nela[$ec]['txt']) && ($nela[$ec]['txt'] != '|'))
							{
								$eline=0;
								break;
							}
							$ec += $nela[$ec]['cs'];
						}
					}
//echo('$eline='.$eline.'<hr />');

					if(!$eline || ($az+1 < $mxz))
					{
						$xc1=dpdf_get_start_x($sts, $xmax);
						$yc1=$yc2;
						$lhs=false;
						if($eline)
							$yc2=$yc1 + $pdf_array['yd'];
						else
						{
							$yhadd=$pdf_array['fs']*($pdf_lines[$az]+1-$lto);
							if(fxIsArray($pdf_lineheight) && isset($pdf_lineheight[$az]) && ($pdf_lineheight[$az] > $yhadd))
							{
								$yhadd=$pdf_lineheight[$az];
								$lhs=true;
							}
							$yc2=$yc1 + $pdf_array['yd']*2.0 + $yhadd;
						}
						$max_y=$pdf_array['h_body']-$pdf_array['h_sum']-$pdf_array['h_foot']-$pdf_space;
						// Passt der ganze Abschnitt noch auf diese Seite?
						// ...ja
						if($yc2 <= $max_y)
						{
							$lmode	= 0;
							$lton	= 1;
						}
						// ...nein
						else
						{
//echo('<b>'.$az.'</b>: $yc1='.$yc1.', $yhadd='.$yhadd.' ('.$pdf_lines[$az].'), $yc2='.$yc2.', $max_y='.$max_y.'<br />');
							// Teilabschnitt ermitteln
							$lmode	= 1;
							$lton	= $lto + floor(($max_y - $yc1 - $pdf_array['yd']*2.0) / $pdf_array['fs']);
							$yc2	= $yc1 + $pdf_array['yd']*2.0 + $pdf_array['fs']*($lton-$lto);
							// ...falls auch dafür kein Platz mehr ist, dann gleich auf neue Seite
							if($lhs || ($lton <= $lto))
							{
								$lton	= $lto;
								$yc2	= $yc1;
								$wexit	= true;
								break;
							}
//echo('&nbsp;&nbsp;&nbsp;-&gt;&nbsp;$lmode='.$lmode.', $lto='.$lto.', $lton='.$lton.', $yc2='.$yc2.'<br />');
						}

						$sc=1;
						$lo='';
						$pa_cs=0;		// Colspans
						$spalte=$sts;	// Startspalte
						$sexit=false;	// Spaltenanzeige vorzeitig verlassen

						// Prüfen, ob ein Colspan-Feld
						$rsp=$rspalte[$spalte];
						if($print_array[$az][$rsp]['bcs'])
						{
							$nsc=(int)$print_array[$az][$rsp]['bcs'];
							while(($spalte >= 0) && ($rsp >= $nsc))
							{
								--$spalte;
								$rsp=$rspalte[$spalte];
								$xc1 -= $pdf_col_width[$rsp];
							}
						}

						while(($spalte < $mxs) && !$sexit)
						{
							if($pa_cs > 1)
							{
								--$pa_cs;
								$spalte++;
								continue;
							}

							$rsp=$rspalte[$spalte];
							$xc2=$xc1+$pdf_col_width[$rsp];
							// Colspans berücksichtigen
							$pa_cs=$print_array[$az][$rsp]['cs'];
							if($pa_cs > 1)
							{
								$cs=$pa_cs;
								$nas=$spalte+1;
								while($cs > 1)
								{
									$xc2 += $pdf_col_width[$rspalte[$nas]];
//echo('$cs: '.$cs.', $nas: '.$nas.', $xc1: '.$xc1.' -> $xc2: '.$xc2.'<BR>');
									$nas++;
									--$cs;
								}
							}
							if($xc2 > $pdf_array['w_body'])
								$sexit=true;

							$value=$print_array[$az][$rsp];
//fxDebug($value, 'Zeile: '.$az.', Spalte: '.$spalte.'('.$rsp.') -- Zeile '.__LINE__);
//echo('<B>value</B>="'.fxHtmlEncode($value['show']).'"<BR>'); // DEL
							$fc=substr($value['show'], 0, 1);
							if(fxIsArray($print_array) && fxIsArray($print_array[$az-1]) && fxIsArray($print_array[$az-1][$rsp]) && isset($print_array[$az-1][$rsp]['show']))
								$oc=substr($print_array[$az-1][$rsp]['show'], 0, 1);
							else
								$oc='';

							$id='';
							$brp=strpos($value['show'],'<brd:');
							if($brp !== false)
							{
								$bre=strpos($value['show'],'>',$brp);
								$id=substr(substr($value['show'],$brp+5,$bre-$brp-5).'0000',0,4);
								$value['show']=substr($value['show'],0,$brp).substr($value['show'],$bre+1);
//echo('<B>value</B>="'.fxHtmlEncode($value['show']).'"<BR>'); // DEL
								$print_array[$az][$rsp]['show']=$value['show'];
							}

							if(!strlen($id))
							{
								// Links
								if(($sc == 1) || ($fc != '|'))
									$id='1';
								else if(($fc == '|') && (substr($print_array[$az][$rsp-1]['show'], 0, 1) != '|'))
									$id='1';
								else
									$id='0';
								// Oben
								if($az == $stz)
								{
									if($uebertrag)
										$id .= '0';
									else
										$id .= '1';
								}
								else if(($fc == '^') && ($lo == '1'))
									$id .= '1';
								else if(($fc == '|') && ($oc != '|'))
									$id .= '1';
								else if(($fc != '^') && ($oc != '|'))
									$id .= '1';
								else if(($fc != '|') && ($fc != '^') && ($oc == '|'))
									$id .= '1';
								else
									$id .= '0';
								$lo=substr($id, -1);
								// Rechts + Unten
								$id .= '00';
							}

							if($pdf_noborders)
							{
								$id='0000';
								if($sc == 1)
									$id='1000';
								if($az == $stz)
									$id=substr($id, 0, 1).'100';
							}

							if($pdf_subwidth || $pdf_subheight)
							{
								if(is_array($pdf_columns[$az][$rsp]) && (strlen($pdf_columns[$az][$rsp][1][0]['txt']) > 1))
									$id='1111';
								else
									$id='0000';
							}
//echo('$id="<B>'.$id.'</B>", $az="<B>'.$az.'</B>", $spalte="<B>'.$spalte.'('.$rsp.')</B>", $sc="<B>'.$sc.'</B>", $mxs="<B>'.$mxs.'</B>", $mxz="<B>'.$mxz.'</B>"<BR>');

							// Box: Hintergrundfarbe + Rahmenfarbe
							$bcol=strtolower($print_array[$az][$rsp]['bcol']);
							if(!$bcol || ($bcol == 'f5f5f6'))
								$bcol='ffffff';
							$fcol=strtolower($print_array[$az][$rsp]['fcol']);
							if(!$fcol)
								$fcol='888888';
							fxpdf_box($pdf, '!'.($xc1+$pdf_subwidth2), $yc1+$pdf_subheight2, '!'.($xc2-$pdf_subwidth2), $yc2-$pdf_subheight2, $bcol, $id, 0.0, 0.0, 0.35, $fcol);

							// PSP: Abhängigkeits-Linie
							if(($_GET['tr'] == 84) && ($id != '0000') && $az)
							{
								$pyo=$yc1-$pdf_subheight2+0.15*$pdf_array['fac'];
								$pyu=$yc1+$pdf_subheight2-0.15*$pdf_array['fac'];
								$pxu=($xc1+$xc2)/2.0;
								fxpdf_line($pdf, '!'.$pxu, $pyo, '!'.$pxu, $pyu);
							}

							if(($fc != '|') && ($fc != '^') && ($fc != '~'))
							{
								// Übertrag bzw. Gesamtsumme
								if($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$rsp]['anzeigeart'] == 14) // Summe
								{
									$ffc=substr($value['fw'], 0, 1);
									$flc=substr($value['fw'], -1);
									if(($ffc != '~') && ($flc != '^') && ($flc != '|') && ($flc != '~') && ($flc != '>'))
									{
										$sid=$az.'_'.$rsp;
										if(!isset($sumset[$sid]))
										{
											$sumset[$sid]=get_double($value['fw']);
											$summe[$rsp] += $sumset[$sid];
//echo('<div style="background:#ffffff;padding:4px;border:1px solid #666666;border-radius:4px;margin-top:2px;"><b>'.__LINE__.'</b> $az='.$az.', $spalte='.$spalte.': $summe['.$rsp.'] + '.get_double($value['fw']).'='.$summe[$rsp].'</div>');
										}
									}
								}
								if(is_array($pdf_columns[$az][$rsp]))
								{
									foreach($pdf_columns[$az][$rsp] as $ziz => $pdfc)
									{
										if(($ziz < $lto) || (($lton > $lto) && ($ziz >= $lton)))
											continue;

										$nziz=$ziz-$lto+1;
										$gesamtbreite=0.00;
										foreach($pdfc as $siz => $pdfsc)
										{
											if($pdfsc['typ'] < 2)	// 0=Text oder 1=Grafik
												$gesamtbreite += $pdfsc['w'];
										}
//echo('<B><FONT COLOR="#000088">$gesamtbreite:</FONT></B> '.$gesamtbreite.'<BR>');
										foreach($pdfc as $siz => $pdfsc)
										{
//echo('<B>-> $pdfsc</B>="'.fxHtmlEncode($pdfsc['txt']).'" ($xc1: '.$xc1.', $xc: '.$xc.', $xc2: '.$xc2.')<BR>'); // DEL
											// Align
											if(strlen($pdfsc['align']))
												$align=$pdfsc['align'];
											else if(strlen($value['align']))
												$align=strtolower($value['align']);
											else
												$align=strtolower($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$rsp]['horausr']);

											if($align == 'right')
												$xc=$xc=$xc2-$pdf_array['xd']-$pdf_subwidth2;
											else if($align == 'center')
												$xc=($xc1+$xc2)/2.0;
											else
												$xc=$xc1+$pdf_array['xd']+$pdf_subwidth2;

											if(!$pdfsc['typ']) // 0=Text
											{
												if(substr($pdfsc['txt'], 0, 4) == '{HR:') // Horizontale Linie
												{
													$hrcolor	= substr($pdfsc['txt'], 4, 6);
													$hry		= $yc1+$pdf_array['fs'];

													fxpdf_line($pdf, '!'.($xc1+$pdf_array['xd']*2.0), $hry, '!'.($xc2-$pdf_array['xd']*2.0), $hry, 0.00, 0.00, 0.35, $hrcolor);
												}
												else
												{
													// Mode: normal, bold, italic
													$mode=$pdfsc['md'];

													// Background?
													$brgb='';
													if(strlen($pdfsc['bcol']))
													{
														$brgb=$pdfsc['bcol'];
														$boxy1=$yc1+($pdf_array['fs']*$nziz)-$pdf_array['yd'];
														$boxy2=$boxy1+$pdf_array['fs'];
														fxpdf_box($pdf, '!'.($xc1+$pdf_array['xd']*0.50), $boxy1, '!'.($xc2-$pdf_array['xd']*0.50), $boxy2, $brgb, '0000');
													}
													$rgb='000000';
													if(strlen($pdfsc['col']))
														$rgb=$pdfsc['col'];
//function fxpdf_text(&$pdf, $text, $xc, $yc, $sf=8.0, $align='left', $mode='normal', $lw=0.25, $rgb='000000', $deg=0, $space=0.0)
													fxpdf_text($pdf, $pdfsc['txt'], '!'.($xc+$pdfsc['xc']), $yc1+($pdf_array['fs']*$nziz)+$pdf_array['yd']+$pdf_subheight2, $pdfsc['fs'], $align, $mode, 0.25, $rgb);
//echo('<div style="padding-left:24px;border-radius:4px;padding:2px;margin-bottom:1px;width:1024px;background:#e8e8e8;"><b>Draw TEXT</b> x=!'.($xc+$pdfsc['xc']).', y='.($yc1+($pdf_array['fs']*$nziz)+$pdf_array['yd']+$pdf_subheight2).', rgb='.$rgb.', brgb='.$brgb.' <i class="blue">(LINE '.__LINE__.')</i><br />'); fxDebug($pdfsc,'$pdfsc'); echo('</div>');
												}

												// Strike
												if($pdfsc['st'])
												{
													$ly=$yc1+$pdf_array['fs'];
													fxpdf_line($pdf, '!'.($xc1+$pdf_array['xd']*2.0), $ly, '!'.($xc2-$pdf_array['xd']*2.0), $ly, 0.00, 0.00, 0.35);
												}
											}
											else if(($pdfsc['typ'] == 1) || ($pdfsc['typ'] == 2))// 1=Grafik oder 2=Dummygrafik
											{
												$iyc=$yc1;
												$frgb='';
												if((($pdfsc['typ'] == 1) && (substr($pdfsc['txt'], -10, 6) == 'bar_bg')) || ($pdfsc['typ'] == 2))
												{
													$iyc += ($pdf_array['fs']-$pdfsc['h']*2/3);
													if($pdfsc['typ'] == 1)
														$frgb='a0a0a0';
												}
												if(!strlen($pdfsc['align']))
												{
													$align='left';
													$alp=stripos($pdfsc['otxt'],' align=');
													if($alp !== false)
													{
														$als=strtolower(substr($pdfsc['otxt'],$alp+7,1));
														if($als == '"')
															$als=strtolower(substr($pdfsc['otxt'],$alp+8,1));
														if($als == 'r')
															$align='right';
														else if(($als == 'c') || ($als == 'm'))
															$align='center';
													}
													$pdfsc['align']=$align;
												}
												if($pdfsc['align'] == 'right')
													$ixc=$xc2-$pdf_array['xd']-$pdf_subwidth2-$pdfsc['xc'];
												else if($pdfsc['align'] != 'left')
													$ixc=($xc1+$xc2)/2.0+$pdfsc['xc'];
												else
													$ixc=$xc+$pdfsc['xc'];
												if(substr($pdfsc['txt'],-15) == 'atc_c_80x80.png')
												{
													$ixc=$xc2+5.5;
													$iyc=$yc1-20.4;
												}
												fxpdf_picture($pdf, $pdfsc['txt'], '!'.$ixc, $iyc+($pdf_array['fs']*($nziz-1)), 1.0, $pdfsc['align'], $pdfsc['w'], $pdfsc['h'], $frgb);
//echo('<div style="padding-left:24px;border-radius:4px;padding:2px;margin-bottom:1px;width:1024px;background:#e8e8e8;"><b>Draw PICTURE</b> x=!'.($xc+$pdfsc['xc']).', y='.($iyc+($pdf_array['fs']*($nziz-1))).', w='.$pdfsc['w'].', h='.$pdfsc['w'].' <i class="blue">(LINE '.__LINE__.')</i><br />'); fxDebug($pdfsc,'$pdfsc', 0); echo('</div>');
											}
										}
									}
								}
							}
							$xc1=$xc2;
							$sc++;
							$spalte++;
						}
					}

					if(!$lmode)
						$az++;
					else
						$wexit=true;

					$lto=$lton;
				}
				$stz=$az;

				// Übertrag bzw. Gesamtsumme -- Y-Koordinate anpassen
				if($az < $mxz)
					dpdf_sum($pdf, 0);	// Übertrag
				else
					dpdf_sum($pdf, 1);	// Gesamtsumme

				// Linie links
				if(($seite == 1) || $pdf_subwidth || $pdf_subheight)
					fxpdf_line($pdf, '!'.(-$xmax), $syc1, '!'.(-$xmax), $yc2, 0.0, 0.0, 0.35, '666666');

				// Schatten zeichen
				if(($stz > $ostz) || $lmode)
				{
					// Rechts
					if(!$nxmax)
					{
						if($config != 3)
						{
							fxpdf_box($pdf, '!'.($xc2+0.25*$pdf_array['fac']), $syc1+2.25*$pdf_array['fac'], '!'.($xc2+2.25*$pdf_array['fac']), $yc2+0.25*$pdf_array['fac'], 'eeeeee', '0000');
							fxpdf_line($pdf, '!'.$xc2, $syc1, '!'.$xc2, $yc2, 0.0, 0.0, 0.35, '666666');
						}
						else
						{
							fxpdf_box($pdf, '!'.($xc2+0.25*$pdf_array['fac']), $syc1+5.00*$pdf_array['fac'], '!'.($xc2+2.25*$pdf_array['fac']), $yc2+0.25*$pdf_array['fac'], 'eeeeee', '0000');
							fxpdf_line($pdf, '!'.$xc2, $syc1+2.75*$pdf_array['fac'], '!'.$xc2, $yc2, 0.0, 0.0, 0.35, '666666');
						}
					}
					// Unten
					fxpdf_box($pdf, '!'.(-$xmax+2.25*$pdf_array['fac']), $yc2+0.25*$pdf_array['fac'], '!'.($xc2+2.25*$pdf_array['fac']), $yc2+2.75*$pdf_array['fac'], 'eeeeee', '0000');
					fxpdf_line($pdf, '!'.(-$xmax), $yc2, '!'.$xc2, $yc2, 0.0, 0.0, 0.35, '666666');
				}
			}
			else
				$yc2=$yc1;

			// Überflüssiges abschneiden
			$cutcol='ffffff';
			if($subseite)	// Links
			{
				fxpdf_box($pdf, '&0.0', '&0.0', '!'.(-0.25*$pdf_array['fac']), '&'.$pdf_array['height'], $cutcol, '0000');
				fxpdf_line($pdf, 0.0, '&0.0', 0.0, '&'.$pdf_array['height'], 2.0*$pdf_array['fac'], 0.5*$pdf_array['fac'], -0.15);
			}
			if($nxmax)	// Rechts
			{
				fxpdf_box($pdf, $pdf_array['w_body']+0.25*$pdf_array['fac'], '&0.0', '&'.$pdf_array['width'], '&'.$pdf_array['height'], $cutcol, '0000');
				if($sxc1 > -9999)
					fxpdf_line($pdf, '!'.$sxc1, '&0.0', '!'.$sxc1, '&'.$pdf_array['height'], 2.0*$pdf_array['fac'], 0.5*$pdf_array['fac'], -0.15);
				else
					fxpdf_line($pdf, $pdf_array['w_body'], '&0.0', $pdf_array['w_body'], '&'.$pdf_array['height'], 2.0*$pdf_array['fac'], 0.5*$pdf_array['fac'], -0.15);
			}

			$yc1=$yc2;

			// Passt der Rechnungsfuss noch auf die Seite
			$ifoot=true;
			if(($stz >= $mxz) && $pdf_array['h_cfoot'])
			{
				$yc2=$yc1+$pdf_array['h_cfoot'];
				if($yc2 <= ($pdf_array['h_body']-$pdf_array['h_foot']-$pdf_space))
				{
					$yc2=dpdf_cfoot($pdf, 1, $yc1);
//fxpdf_rec($pdf, 0.0, $yc1, $pdf_array['w_body'], $yc2);
					$nxt=false;
				}
				else
				{
					$nxt=true;
					$ifoot=false;
				}
			}
			else if($stz >= $mxz)
				$nxt=false;

			// Passt normaler Fuss noch auf die Seite
			$nfoot=true;
			if(!$nxt || !$ifoot)
			{
				if(($stz >= $mxz) && $pdf_array['h_foot'])
				{
					$yc2=$yc1+$pdf_array['h_foot'];
					if($yc2 <= ($pdf_array['h_body']-$pdf_space))
					{
						if($ifoot)
							$nxt=false;
					}
					else
					{
						$nxt=true;
						$nfoot=false;
					}
				}
				else if(($stz >= $mxz) && $ifoot)
					$nxt=false;
			}

			// Storniert
			if($rechnung_array['archiv'] == 1)
				fxpdf_diagonal($pdf, false);

			// Fuss
			if((!$nxt || $nfoot) && $pdf_array['h_foot'])
			{
				fxpdf_clipping($pdf, 1, false, true);
				if($pdf_array['foot'])
				{
					$pdf_array['foot']=dpdf_user_substitute($pdf, $pdf_array['foot'], meldung(1037));	// Leistungsnachweis etc.
					fxpdf_user($pdf, $pdf_array['foot']);
				}
			}

			// Testversion
			if($testversion)
				fxpdf_diagonal($pdf, true);

			// Aktuelle Seite abschliessen, so dass keine Änderungen mehr vorgenommen werden können.
			// Vorher noch die FeRox-Fusszeile setzen
			if($config)
				$snr=$seite;
			else
				$snr=$seite.chr(65+$subseite);
			if(!isset($pdf_array['f_nofoot']))
				fxpdf_clipping($pdf, 2, $snr, true, $dmb);
			fxpdf_page($pdf, 'end');
			$seite++;
		}
		$xmax=$nxmax;
		if($xmax)
			$sts=$nsts;
		else
			$sts=$mxs;
		$subseite++;
//echo('<HR>$sts='.$sts.', $xmax='.$xmax.'<BR>');
	}

	// Tooltips als Anhang
	if(is_array($pdf_tooltips))
	{
		ksort($pdf_tooltips);
//fxDebug($pdf_tooltips, 'Anhang');

		$trans_html['<br />'.chr(13).chr(10)]='|';
		$trans_html['<br />'.chr(10).chr(13)]='|';
		$trans_html['<br />'.chr(13)]='|';
		$trans_html['<br />'.chr(10)]='|';
		$trans_html['<br />']='|';

		$trans_html['<br>'.chr(13).chr(10)]='|';
		$trans_html['<br>'.chr(10).chr(13)]='|';
		$trans_html['<br>'.chr(13)]='|';
		$trans_html['<br>'.chr(10)]='|';
		$trans_html['<br>']='|';

		$trans_html[chr(13).chr(10)]='|';
		$trans_html[chr(10).chr(13)]='|';
		$trans_html[chr(13)]='|';
		$trans_html[chr(10)]='|';

		$trans_html[':&nbsp;']=':~';
		$trans_html[':'.chr(9)]=':~';
		$trans_html[chr(9)]='~';

		$trans_html['&nbsp;']=' ';
		$trans_html['&quot;']='"';

		$trans_html['&apos;']="'";

		$nla=array(chr(10)=>'#', chr(13)=>'*');

		$ttc=0;
		$ntt='';
		foreach($pdf_tooltips as $tt => $ttdat)	// Alle Tooltipzeilen durchgehen und Array erstellen
		{
			$tooltip=strtr($ttdat, $trans_html);
			$tex=explode('|', $tooltip);
			if(is_array($tex))
			{
				$ntt[$ttc]['head']=no_null($tt);
				$ntt[$ttc]['mode']='bold';

				$ntex='';
				foreach($tex as $key => $value)
				{
					// Literal vorhanden
					$ltp=strpos($value, ':~');
					if($ltp !== false)	// Ja
					{
						$ntex[$key]['lit']=trim(substr($value, 0, $ltp+1));
						$nval=trim(substr($value, $ltp+2));
					}
					else
						$nval=trim($value);
					$ntex[$key]['val']=str_replace('~', '', $nval);
				}
				$ntt[$ttc]['tip']=$ntex;
			}
			$ttc++;
		}
		fxpdf_anhang($pdf, $ntt);
	}

	// Aktuelles PDF-Dokument schliessen
	$pdf_data=fxpdf_page($pdf, 'close');
}

// CSV bereinigen?
if(!isset($_preview_config) && fxIsArray($GLOBALS['csv_array']))
{
	// Leere Spalten?
	$hma=array();
	$cca=array();
	$lcol=$GLOBALS['csv_array']['lcol'];
	if(fxIsArray($GLOBALS['csv_array']['lines']))
	{
		foreach($GLOBALS['csv_array']['lines'] as $l => $ca)
		{
			for($c=0; $c<=$lcol; $c++)
			{
				if(!isset($cca[$c]))
					$cca[$c]=0;
				if(isset($ca[$c]) && strlen($ca[$c]))
				{
					$cca[$c]++;
//echo('['.$l.']['.$c.'] -- &quot;'.$ca[$c].'&quot; --&gt; '.$cca[$c].'<br />');
				}
			}
		}
	}
//fxDebug($cca,'$cca');
	if(sizeof($cca))
	{
		foreach($cca as $c => $ce)
		{
			if(!$ce)
			{
				$hma[$c]=true;
				if(fxIsArray($GLOBALS['csv_array']['headers']) && isset($GLOBALS['csv_array']['headers'][$c]))
					unset($GLOBALS['csv_array']['headers'][$c]);
			}
		}
	}
//fxDebug($hma,'$hma');
	$hca=array();
	if(sizeof($hma))
	{
		$cok=-1;
		for($c=0; $c<=$lcol; $c++)
		{
			if(isset($hma[$c]))
			{
				if($cok < 0)
					$cok=$c;
			}
			else if($cok >= 0)
			{
				$hca[$c]=$cok;
				$cok++;
			}
		}
	}
//fxDebug($hca,'$hca');
	if(sizeof($hca))
	{
		$GLOBALS['csv_array']['lcol']=0;
		foreach($hca as $oc => $nc)
		{
			if(fxIsArray($GLOBALS['csv_array']['headers']))
			{
				if(isset($GLOBALS['csv_array']['headers'][$oc]))
				{
					$GLOBALS['csv_array']['headers'][$nc]=$GLOBALS['csv_array']['headers'][$oc];
					unset($GLOBALS['csv_array']['headers'][$oc]);
					$GLOBALS['csv_array']['lcol']=max($GLOBALS['csv_array']['lcol'],$nc);
				}
			}
			if(fxIsArray($GLOBALS['csv_array']['units']))
			{
				if(isset($GLOBALS['csv_array']['units'][$oc]))
				{
					$GLOBALS['csv_array']['units'][$nc]=$GLOBALS['csv_array']['units'][$oc];
					unset($GLOBALS['csv_array']['units'][$oc]);
				}
			}
			if(fxIsArray($GLOBALS['csv_array']['lines']))
			{
				foreach($GLOBALS['csv_array']['lines'] as $l => $ca)
				{
					if(isset($ca[$oc]))
					{
						$GLOBALS['csv_array']['lines'][$l][$nc]=$GLOBALS['csv_array']['lines'][$l][$oc];
						unset($GLOBALS['csv_array']['lines'][$l][$oc]);
					}
				}
			}
		}
	}
}

// Spreadsheet
if(strlen($GLOBALS['_ext_spreadsheet']) && !isset($_preview_config) && fxIsArray($GLOBALS['csv_array']) && fxIsArray($GLOBALS['print_array']) && !isset($_GET['no_excel']))
{
//fxDebug($GLOBALS['csv_array'],'csv_array');
//fxDebug($GLOBALS['print_array'],'print_array');

/*
	// Excel: Create new PHPExcel object
	$pe=new PHPExcel();

	// Excel... set document properties
	$headline='';
	$pe->getProperties()->setCreator(fxpExcelUTF8($GLOBALS['fxpglobals']['uname']));
	$pe->getProperties()->setLastModifiedBy(fxpExcelUTF8($GLOBALS['fxpglobals']['uname']));
	if(strlen($GLOBALS['csv_array']['headlines'][0]))
	{
		$headline=fxpExcelUTF8($GLOBALS['csv_array']['headlines'][0]);
		$pe->getProperties()->setTitle($headline);
	}
	if(strlen($GLOBALS['csv_array']['headlines'][1]))
		$pe->getProperties()->setSubject(fxpExcelUTF8($GLOBALS['csv_array']['headlines'][1]));

	// Excel... set default font
	$pe->getDefaultStyle()->getFont()->setName('Arial')->setSize(12);

	// Excel... define default borders
	$headerborder=array('borders'=>array('outline'=>array('style'=>PHPExcel_Style_Border::BORDER_THIN, 'color'=>array('argb'=>'FF000000'))));
	$headerborder_b=array('borders'=>array('bottom'=>array('style'=>PHPExcel_Style_Border::BORDER_THIN, 'color'=>array('argb'=>'FF000000'))));
	$cellborder=array('borders'=>array('left'=>array('style'=>PHPExcel_Style_Border::BORDER_THIN, 'color'=>array('argb'=>'FF777777')), 'right'=>array('style'=>PHPExcel_Style_Border::BORDER_THIN, 'color'=>array('argb'=>'FF777777'))));
	$cellborder_tb=array('borders'=>array('top'=>array('style'=>PHPExcel_Style_Border::BORDER_THIN, 'color'=>array('argb'=>'FF777777')), 'bottom'=>array('style'=>PHPExcel_Style_Border::BORDER_THIN, 'color'=>array('argb'=>'FF777777'))));

	// Excel... add cell data
	$mca=array();
	$bla=array();
	$lcol=(int)$GLOBALS['csv_array']['lcol'];
	$loff=1;

	// Excel... add headline
	if(strlen($headline))
		$loff++;

	// Excel... add headers
	if(fxIsArray($GLOBALS['csv_array']['headers']))
	{
		for($c=0; $c<=$lcol; $c++)
		{
			if(isset($GLOBALS['csv_array']['headers'][$c]))
			{
				$cell=fxpExcelCell($c+1,$loff);
				if(strlen($GLOBALS['csv_array']['headers'][$c]))
				{
					$pe->setActiveSheetIndex(0)->setCellValue($cell, strip_tags(fxpExcelUTF8(strtr($GLOBALS['csv_array']['headers'][$c], $trans))));
					$pe->getActiveSheet(0)->getStyle($cell)->getAlignment()->setVertical(PHPExcel_Style_Alignment::VERTICAL_CENTER);
				}
				$pe->getActiveSheet(0)->getStyle($cell)->getAlignment()->setHorizontal(PHPExcel_Style_Alignment::HORIZONTAL_CENTER);
				$pe->getActiveSheet(0)->getStyle($cell)->getFont()->setBold(true);
				$pe->getActiveSheet(0)->getStyle($cell)->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID)->getStartColor()->setARGB('FFDDDDDD');
				$pe->getActiveSheet(0)->getStyle($cell)->applyFromArray($headerborder);
			}
		}
		$loff++;
	}

	// Excel... add lines
	$line=$loff;
	if(fxIsArray($GLOBALS['csv_array']['lines']))
	{
		foreach($GLOBALS['csv_array']['lines'] as $l => $ca)
		{
			for($c=0; $c<=$lcol; $c++)
			{
				// Cell
				$cell=fxpExcelCell($c+1,$line);

				// Merge
				$ccs=1;
				if(fxIsArray($GLOBALS['print_array'][$l]) && fxIsArray($GLOBALS['print_array'][$l][$c]) && isset($GLOBALS['print_array'][$l][$c]['cs']))
					$ccs=max(1, (int)$GLOBALS['print_array'][$l][$c]['cs']);
				if($ccs > 1)
				{
					$mca[$cell]=array('dcell'=>fxpExcelCell($c+$ccs, $line), 'bold'=>false, 'align'=>"l", 'color'=>"");
					$bla[$line]=true;
				}
				$lcell=max($lcell, $c+$ccs);

				// Format?
				$cfm='';
				if(isset($GLOBALS['csv_array']['units'][$c]))
					$cfm='###,###,##0.00" '.fxpExcelUTF8($GLOBALS['csv_array']['units'][$c]).'"';
				else if(is_double($ctxt))
					$cfm='###,###,##0.00';
				else if(is_integer($ctxt))
					$cfm='###,###,##0';

				// Text
				$lb=false;
				$ctxt='';
				if(isset($ca[$c]))
				{
					$ctxt=$ca[$c];
					if(strlen($ctxt))
					{
						$explicit=false;
						if(substr($ctxt,0,1) == '=')
							$explicit=true;
						$lb=stripos($ctxt,$nlrep);
						if($lb !== false)
							$ctxt=str_ireplace($nlrep, "\n", $ctxt);
						if($explicit)
							$pe->setActiveSheetIndex(0)->setCellValueExplicit($cell, fxpExcelUTF8($ctxt));
						else
							$pe->setActiveSheetIndex(0)->setCellValue($cell, fxpExcelUTF8($ctxt));
						$pe->getActiveSheet(0)->getStyle($cell)->getAlignment()->setVertical(PHPExcel_Style_Alignment::VERTICAL_TOP);
					}
				}

				// Format
				if(strlen($cfm))
					$pe->getActiveSheet(0)->getStyle($cell)->getNumberFormat()->setFormatCode($cfm.';[Red]-'.$cfm);

				// Line break?
				if($lb !== false)
					$pe->getActiveSheet(0)->getStyle($cell)->getAlignment()->setWrapText(true);

				// ...Bold?
				if(fxIsArray($GLOBALS['print_array'][$l]) && fxIsArray($GLOBALS['print_array'][$l][$c]) && fxIsArray($GLOBALS['print_array'][$l][$c]['lines']) && fxIsArray($GLOBALS['print_array'][$l][$c]['lines'][1]) && isset($GLOBALS['print_array'][$l][$c]['lines'][1]['md']) && ($GLOBALS['print_array'][$l][$c]['lines'][1]['md'] == 'bold'))
					$pe->getActiveSheet(0)->getStyle($cell)->getFont()->setBold(true);

				// ...Align?
				if(fxIsArray($GLOBALS['print_array'][$l]) && fxIsArray($GLOBALS['print_array'][$l][$c]) && fxIsArray($GLOBALS['print_array'][$l][$c]['lines']) && fxIsArray($GLOBALS['print_array'][$l][$c]['lines'][1]) && isset($GLOBALS['print_array'][$l][$c]['lines'][1]['align']) && strlen($GLOBALS['print_array'][$l][$c]['lines'][1]['align']))
				{
					$align=strtolower(substr($GLOBALS['print_array'][$l][$c]['lines'][1]['align'],0,1));
					if(isset($mca[$cell]))
						$mca[$cell]['align']=$align;
					if($align == 'c')
						$pe->getActiveSheet(0)->getStyle($cell)->getAlignment()->setHorizontal(PHPExcel_Style_Alignment::HORIZONTAL_CENTER);
					else if($align == 'r')
						$pe->getActiveSheet(0)->getStyle($cell)->getAlignment()->setHorizontal(PHPExcel_Style_Alignment::HORIZONTAL_RIGHT);
					else if($align == 'l')
						$pe->getActiveSheet(0)->getStyle($cell)->getAlignment()->setHorizontal(PHPExcel_Style_Alignment::HORIZONTAL_LEFT);
				}

				// Background
				$bgcol='';
				if(!strlen($bgcol) && fxIsArray($GLOBALS['print_array'][$l]) && fxIsArray($GLOBALS['print_array'][$l][$c]) && fxIsArray($GLOBALS['print_array'][$l][$c]['lines']) && fxIsArray($GLOBALS['print_array'][$l][$c]['lines'][1]) && isset($GLOBALS['print_array'][$l][$c]['lines'][1]['bcol']) && strlen($GLOBALS['print_array'][$l][$c]['lines'][1]['bcol']))
					$bgcol=strtoupper(substr($GLOBALS['print_array'][$l][$c]['lines'][1]['bcol'],0,6));
				if(!strlen($bgcol) && fxIsArray($GLOBALS['print_array'][$l]) && fxIsArray($GLOBALS['print_array'][$l][$c]) && isset($GLOBALS['print_array'][$l][$c]['bcol']) && strlen($GLOBALS['print_array'][$l][$c]['bcol']))
					$bgcol=strtoupper(substr($GLOBALS['print_array'][$l][$c]['bcol'],0,6));
				if(!strlen($bgcol) && fxIsArray($GLOBALS['print_array'][$l]) && fxIsArray($GLOBALS['print_array'][$l][0]) && fxIsArray($GLOBALS['print_array'][$l][0]['lines']) && fxIsArray($GLOBALS['print_array'][$l][0]['lines'][1]) && isset($GLOBALS['print_array'][$l][0]['lines'][1]['bcol']) && strlen($GLOBALS['print_array'][$l][0]['lines'][1]['bcol']))
					$bgcol=strtoupper(substr($GLOBALS['print_array'][$l][0]['lines'][1]['bcol'],0,6));
				if(!strlen($bgcol) && fxIsArray($GLOBALS['print_array'][$l]) && fxIsArray($GLOBALS['print_array'][$l][0]) && isset($GLOBALS['print_array'][$l][0]['bcol']) && strlen($GLOBALS['print_array'][$l][0]['bcol']))
					$bgcol=strtoupper(substr($GLOBALS['print_array'][$l][0]['bcol'],0,6));
				if(strlen($bgcol))
					$pe->getActiveSheet(0)->getStyle($cell)->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID)->getStartColor()->setARGB('FF'.$bgcol);
				else if($l%2)
					$pe->getActiveSheet(0)->getStyle($cell)->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID)->getStartColor()->setARGB('FFEEEEEE');
				else
					$pe->getActiveSheet(0)->getStyle($cell)->getFill()->setFillType(PHPExcel_Style_Fill::FILL_SOLID)->getStartColor()->setARGB('FFFFFFFF');

				// Color
				if(!strlen($cfm) && fxIsArray($GLOBALS['print_array'][$l]) && fxIsArray($GLOBALS['print_array'][$l][$c]) && fxIsArray($GLOBALS['print_array'][$l][$c]['lines']) && fxIsArray($GLOBALS['print_array'][$l][$c]['lines'][1]) && isset($GLOBALS['print_array'][$l][$c]['lines'][1]['col']) && strlen($GLOBALS['print_array'][$l][$c]['lines'][1]['col']))
				{
					$col=strtoupper(substr($GLOBALS['print_array'][$l][$c]['lines'][1]['col'],0,6));
					$pe->getActiveSheet(0)->getStyle($cell)->getFont()->getColor()->setARGB('FF'.$col);
				}

				// Border
				$pe->getActiveSheet(0)->getStyle($cell)->applyFromArray($cellborder);
			}

			$line++;
		}
	}
	if($line > $loff)
		$bla[$line-1]=true;

	// Excel... merge cells
//fxDebug($mca,'$mca');
	if(sizeof($mca))
	{
		foreach($mca as $scell => $dca)
		{
			$mcell=$scell.':'.$dca['dcell'];
			$pe->setActiveSheetIndex(0)->mergeCells($mcell);
			$pe->getActiveSheet(0)->getStyle($mcell)->getAlignment()->setVertical(PHPExcel_Style_Alignment::VERTICAL_CENTER);
			if($dca['align'] == 'c')
				$pe->getActiveSheet(0)->getStyle($mcell)->getAlignment()->setHorizontal(PHPExcel_Style_Alignment::HORIZONTAL_CENTER);
			else if($dca['align'] == 'r')
				$pe->getActiveSheet(0)->getStyle($mcell)->getAlignment()->setHorizontal(PHPExcel_Style_Alignment::HORIZONTAL_RIGHT);
			else if($dca['align'] == 'l')
				$pe->getActiveSheet(0)->getStyle($mcell)->getAlignment()->setHorizontal(PHPExcel_Style_Alignment::HORIZONTAL_LEFT);
		}
	}

	// Excel... set borders
//echo('<div style="margin-top:2px;margin-right:2px;padding:4px;border-radius:4px;background:#eeeeee;">$lcell=<b>'.$lcell.'</b>, $loff=<b>'.$loff.'</b>, $line=<b>'.$line.'</b></div>');
//fxDebug($bla,'$bla');
	if(sizeof($bla))
	{
		foreach($bla as $line  => $dummy)
			$pe->getActiveSheet(0)->getStyle(fxpExcelCell(1,$line).':'.fxpExcelCell($lcell,$line))->applyFromArray($cellborder_tb);
	}
	if(fxIsArray($GLOBALS['csv_array']['headers']))
		$pe->getActiveSheet(0)->getStyle(fxpExcelCell(1,$loff-1).':'.fxpExcelCell($lcell,$loff-1))->applyFromArray($headerborder_b);

	// Excel... get column width
	$wca=array();
	for($c=1; $c<=$lcell; $c++)
	{
		$cc=fxpExcelCell($c);
		$pe->getActiveSheet(0)->getColumnDimension($cc)->setAutoSize(true);
	}
	$pe->getActiveSheet()->calculateColumnWidths();
	for($c=1; $c<=$lcell; $c++)
	{
		$cc=fxpExcelCell($c);
		$wca[$c]=$pe->getActiveSheet(0)->getColumnDimension($cc)->getWidth();
	}

	// Excel... add headline
	if(strlen($headline))
	{
		$cell='A1';
		$pe->setActiveSheetIndex(0)->setCellValue($cell, $headline);
		$pe->getActiveSheet(0)->getStyle($cell)->getAlignment()->setVertical(PHPExcel_Style_Alignment::VERTICAL_TOP);
		$pe->getActiveSheet(0)->getStyle($cell)->getFont()->setSize(18)->setBold(true);
		$pe->getActiveSheet()->getRowDimension(1)->setRowHeight(50.0);
	}

	// Excel... set column width
	foreach($wca as $c => $cw)
	{
		$cc=fxpExcelCell($c);
//echo('<div style="margin-top:2px;margin-right:2px;padding:4px;border-radius:4px;background:#eeeeee;"><b>'.$cc.'</b> width='.$ncw.' ('.$cw.')</div>');
		$pe->getActiveSheet(0)->getColumnDimension($cc)->setAutoSize(false)->setWidth($cw);
	}

	// Excel... freeze pane
	if($loff > 1)
		$pe->getActiveSheet()->freezePane('A'.$loff);

	// Excel... rename worksheet
	if(strlen($GLOBALS['csv_array']['headlines'][0]))
		$pe->getActiveSheet()->setTitle(fxpExcelUTF8($GLOBALS['csv_array']['headlines'][0],true));
	else
		$pe->getActiveSheet()->setTitle('fx-project Excel');

	// Excel... set active sheet index to the first sheet, so Excel opens this as the first sheet
	$pe->setActiveSheetIndex(0);

	// Excel... save Excel 2007
	$GLOBALS['excel_array']=array('file'=>$GLOBALS['tmppath'].'~'.$ad.'.xlsx', 'vers'=>'Excel2007', 'type'=>'xlsx');
	if(file_exists($GLOBALS['excel_array']['file']))
		unlink($GLOBALS['excel_array']['file']);
	$pew=PHPExcel_IOFactory::createWriter($pe,$GLOBALS['excel_array']['vers']);
	$pew->save($GLOBALS['excel_array']['file']);
*/
}

// PDF Dateilänge
if($pdf_data)
	$pdf_len=strlen($pdf_data);

// Einstellungswerte wieder zurücksetzen
$GLOBALS['fxpglobals']['settings']=$settings_backup;

//Dateiname
$csv_tz=REPORTDOWNLOADTRENNZEICHEN;
$fn=urlencode(cleanFilename($pdf_array['filename']));

// Erstellte PDF-Rechnungen im DMS hinterlegen
$pdf_inv_filename='';
$pdf_inv_error=0;
if(!isset($_preview_config) && !$pdfloaded && $rechid && $pdf_len && $GLOBALS['fxpglobals']['client'] && $GLOBALS['fxpglobals']['user'] && $GLOBALS['fxpglobals']['person'])
{
	$orgname=$fn.'.pdf';
	$filename=$GLOBALS['tmppath'].'~'.$ad.'.pdf';
	$pdf_inv_filename=$filename;
//fxDebug($filename,'$filename: $orgname='.$orgname);

	// PDF-Datei in TEMP-Ordner speichern
	$fe=fxSaveBin($filename, $pdf_data);
	if(!$fe)
	{
		// PDF-Datei ins DMS eintragen
		$pid=(int)$rechnung_array['kunde'];
		$rechnung_array['pname']=db_value("SELECT pname FROM personen WHERE personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);

		$did=(int)db_value("SELECT dok_id FROM d_dokumente WHERE elter_art=8 AND dok_art=2580 AND id=".$rechid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
		if($did)
			$rid=(int)db_value("SELECT dok_verlauf_id FROM d_dok_verlauf WHERE dok_id=".$did." AND aktiv_kz=1 AND mandanten_id=".$GLOBALS['fxpglobals']['client']);

		// Dokument-ID
		$seid='8_2580/'.$pid.'.'.$did.'.'.$rid;	// 8_2580=Rechnungen/Ausgangsrechnungen (elter_art=8, dok_art=2580), $pid=Auftraggeber (elter)

		// Upload-Attribute
		$uarr=array(
			'error'		=> 0,
			'name'		=> $orgname,
			'size'		=> filesize($filename),
			'new_name'	=> $filename
		);

		// Dokument-Attribute
		$desc=meldung(872, false); // Rechnung
		$invt=(int)$rechnung_array['rechnungstyp'];
		if($invt > 0)
			$desc .= ' - '.db_value("SELECT kommunikation_txt FROM kommunikation WHERE kommunikation_id=".$invt." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
		$desc .= ' ['.$rechnung_array['rechnn_f'].']';

		$values=array(
			'id'			=> $rechid,	// Rechnungs-ID
			'beschreibung'	=> $desc	// Rechnung [NUMMER]
		);

		// Dokument in das DMS hinzufügen
		if(!class_exists('fxpDoc'))
		{
			$GLOBALS['__includer']['dms']=true;
			require('includer.inc');
		}
		if(class_exists('fxpDoc'))
		{
			$doc=new fxpDoc();
			$rs=$doc->uploadDocument($seid, $uarr, $values);
			if(substr($rs,0,1) == '#')
				$pdf_inv_error=__LINE__;
		}
		else
			$pdf_inv_error=__LINE__;
	}
	else
		$pdf_inv_error=__LINE__;
}

$tr=$_backup_tr;
$GLOBALS['fxpglobals']['tr']=$tr;
$lang=$_backup_lang;
$GLOBALS['fxpglobals']['lang']=$lang;

if(!isset($_GET['just_create']))
{
	// Download-Manager aufrufen
	if(isset($_preview_config))
	{
		// PDF-Dokument speichern und Redirect zum direkten Anzeigen
		$fs=$GLOBALS['tmppath'].'~'.$ad.'.pdf';
		$fe=fxSaveBin($fs, $pdf_data);
		if(!$fe)
			echo('<head><meta http-equiv="refresh" content="0;URL=loader.php?url=config_preview.inc&pf='.$ad.'&config='.$config.'"></head>');
		die;
	}
	else
	{
		dm_window($fn,$ad);
	}
}


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  DPDF_USER_SUBSTITUTE                                                     +
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf      - Mandatory parameter:                ???PARAMETER???
 * @param various $orderline - Mandatory parameter:                ???PARAMETER???
 * @param string  $otitle    - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dpdf_user_substitute(&$pdf, $orderline, $otitle='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $pdf_array, $rechnung_array;

	// Variablen für Ersetzungswerte füllen
	// ...!title
	$otitle=trim($otitle);
	if(strlen($otitle))
		$otitle=strtoupper($otitle);
	else
		$otitle='REPORT';
	// ...!project
	if(isset($GLOBALS['fxptdata']['backup']['pdf']) && is_array($GLOBALS['fxptdata']['backup']['pdf']) && isset($GLOBALS['fxptdata']['backup']['pdf']["prj"]))
		$prj=(int)$GLOBALS['fxptdata']['backup']['pdf']["prj"];
	else if(($GLOBALS['config'] == 1) && isset($GLOBALS['fxptdata']['backup']) && is_array($GLOBALS['fxptdata']['backup']) && isset($GLOBALS['fxptdata']['backup']["Projekt_ID"]))
		$prj=(int)$GLOBALS['fxptdata']['backup']["Projekt_ID"];
	else
		$prj=(int)$GLOBALS['fxpglobals']['project'];
	$txt_prj='';
	if($prj)
	{
		$sql="SELECT projekt_id, vorgangsnummer, name_projekt FROM projekte WHERE projekt_id IN (".superior_vn($prj).") ORDER BY vorgangsnummer";
		$tmp=db_values($sql, '*');
		if(is_array($tmp))
		{
			foreach($tmp as $apa)
			{
				if($apa['projekt_id'] == $prj)
					$vn=no_null($apa['vorgangsnummer']).': ';
				else
					$vn='';
				if(strlen($txt_prj))
					$txt_prj .= ' -> ';
				$txt_prj .= $vn.$apa['name_projekt'];
			}
			if(strlen($txt_prj) > 70)
				$txt_prj='...'.substr($txt_prj, -66);
		}
		else
			$txt_prj='ID: '.$prj;
	}
	// ...!name
	if(isset($GLOBALS['fxptdata']['backup']['pdf']) && is_array($GLOBALS['fxptdata']['backup']['pdf']) && isset($GLOBALS['fxptdata']['backup']['pdf']["pers"]))
		$pers=(int)$GLOBALS['fxptdata']['backup']['pdf']["pers"];
	else if(($GLOBALS['config'] == 1) && isset($GLOBALS['fxptdata']['backup']) && is_array($GLOBALS['fxptdata']['backup']) && isset($GLOBALS['fxptdata']['backup']["Personen_ID"]))
		$pers=(int)$GLOBALS['fxptdata']['backup']["Personen_ID"];
	else
		$pers=$GLOBALS['fxpglobals']['person'];
	$txt_pers='';
	if($pers)
	{
		$tmp=db_values("SELECT pname, vorname FROM personen WHERE personen_id=".$pers." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
		$txt_pers=trim($tmp['pname']);
		if(strlen(trim($tmp['vorname'])))
			$txt_pers .= ', '.trim($tmp['vorname']);
	}
	// ...!customer
	if(isset($GLOBALS['fxptdata']['backup']['pdf']) && is_array($GLOBALS['fxptdata']['backup']['pdf']) && isset($GLOBALS['fxptdata']['backup']['pdf']["cust"]))
		$cust=(int)$GLOBALS['fxptdata']['backup']['pdf']["cust"];
	else if(($GLOBALS['config'] == 1) && isset($GLOBALS['fxptdata']['backup']) && is_array($GLOBALS['fxptdata']['backup']) && isset($GLOBALS['fxptdata']['backup']["Kunde"]))
		$cust=(int)$GLOBALS['fxptdata']['backup']["Kunde"];
	else
		$cust=0;
	$txt_cust='';
	if($cust)
	{
		$tmp=db_values("SELECT pname, vorname FROM personen WHERE personen_id=".$cust." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
		$txt_cust=trim($tmp['pname']);
		if(strlen(trim($tmp['vorname'])))
			$txt_cust .= ', '.trim($tmp['vorname']);
	}
	// ...!timespan
	$sdate='';
	$edate='';
	if(isset($GLOBALS['fxptdata']['backup']['pdf']) && is_array($GLOBALS['fxptdata']['backup']['pdf']) && isset($GLOBALS['fxptdata']['backup']['pdf']["sdate"]))
	{
		$dfsl=strlen($GLOBALS['fxptdata']['backup']['pdf']["sdate"]);
		if($dfsl)
		{
			if(($dfsl == 8) || ($dfsl == 14))
				$sdate=get_entry($GLOBALS['fxptdata']['backup']['pdf']["sdate"], "datum");
			else
				$sdate=$GLOBALS['fxptdata']['backup']['pdf']["sdate"];
		}
		$dfsl=strlen($GLOBALS['fxptdata']['backup']['pdf']["edate"]);
		if($dfsl)
		{
			if(($dfsl == 8) || ($dfsl == 14))
				$edate=get_entry($GLOBALS['fxptdata']['backup']['pdf']["edate"], "datum");
			else
				$edate=$GLOBALS['fxptdata']['backup']['pdf']["edate"];
		}
	}
	else if(($GLOBALS['config'] == 1) && isset($GLOBALS['fxptdata']['backup']) && is_array($GLOBALS['fxptdata']['backup']) && isset($GLOBALS['fxptdata']['backup']["Zeitspanne_von"]))
	{
		$dfsl=strlen($GLOBALS['fxptdata']['backup']["Zeitspanne_von"]);
		if($dfsl)
		{
			if(($dfsl == 8) || ($dfsl == 14))
				$sdate=get_entry($GLOBALS['fxptdata']['backup']["Zeitspanne_von"], "datum");
			else
				$sdate=$GLOBALS['fxptdata']['backup']["Zeitspanne_von"];
		}
		$dfsl=strlen($GLOBALS['fxptdata']['backup']["Zeitspanne_bis"]);
		if($dfsl)
		{
			if(($dfsl == 8) || ($dfsl == 14))
				$edate=get_entry($GLOBALS['fxptdata']['backup']["Zeitspanne_bis"], "datum");
			else
				$edate=$GLOBALS['fxptdata']['backup']["Zeitspanne_bis"];
		}
	}
	$txt_timespan='';
	if(strlen($sdate) || strlen($edate))
	{
		if(strlen($sdate))
			$txt_timespan .= $sdate.' ';
		$txt_timespan .= '- ';
		if(strlen($edate))
			$txt_timespan .= $edate;
	}

	// Spezialersetzungen
	// ...!title
	$orderline=str_replace('^!title', '^T<!title>|1.0|20.0|18|left|bold', $orderline);
	$orderline=str_replace('<!title>', $otitle, $orderline);
	// ...!data
	$data_x=95.0; $data_y=50.0;
	$data ='^T'.meldung(1094, true).':|1.0|'.$data_y.'|11|left|bold^T<!name>|'.$data_x.'|'.$data_y.'|10|left|normal';		// Mitarbeiter
	$data_y += 13.0;
	$data .= '^T'.get_text(814, '20').':|1.0|'.$data_y.'|11|left|bold^T<!project>|'.$data_x.'|'.$data_y.'|10|left|normal';	// Projekt
	$data_y += 13.0;
	$data .= '^T'.get_text(120, '15').':|1.0|'.$data_y.'|11|left|bold^T<!customer>|'.$data_x.'|'.$data_y.'|10|left|normal';	// Auftraggeber
	$data_y += 13.0;
	$data .= '^T'.meldung(1105, true).':|1.0|'.$data_y.'|11|left|bold^T<!timespan>|'.$data_x.'|'.$data_y.'|10|left|normal';	// Zeitspanne
	$orderline=str_replace('^!data', $data, $orderline);
	// ...!sign
	$sign_y=-25.0;
	$sign ='^L1.0|'.$sign_y.'|'.(($pdf_array['w_body'] / 2.0) - 10.0).'|'.$sign_y.'|1.0|1.0';
	$spers=get_text(121, '20');																							// Projektleiter
	if($GLOBALS['config'] != 1)
		$spers .= ' / '.get_text(770, '20');																				// Vorgesetzter
	$sign .= '^T'.$spers.'|1.0|'.($sign_y + 8.0).'|8|left|normal';
	$sign .= '^L'.(($pdf_array['w_body'] / 2.0) + 10.0).'|'.$sign_y.'|-1.0|'.$sign_y.'|1.0|1.0';
	$sign .= '^T'.meldung(1483, true);																						// Leistungserbringer
	if((isset($GLOBALS['_preview_subst']) && is_array($GLOBALS['_preview_subst']) && isset($GLOBALS['_preview_subst']['name'])) || strlen($txt_pers))
		$sign .= ' (<!name>)';
	$sign .= '|'.(($pdf_array['w_body'] / 2.0) + 10.0).'|'.($sign_y + 8.0).'|8|left|normal';
	$sign_y -= 25;
	$sign .= '^L1.0|'.$sign_y.'|'.(($pdf_array['w_body'] / 2.0) - 10.0).'|'.$sign_y.'|1.0|1.0';
	$ort_datum=get_text(35, '20').', '.get_text(273, '20');																// Ort, Datum
	$sign .= '^T'.$ort_datum.'|1.0|'.($sign_y + 8.0).'|8|left|normal';
	$sign .= '^L'.(($pdf_array['w_body'] / 2.0) + 10.0).'|'.$sign_y.'|-1.0|'.$sign_y.'|1.0|1.0';
	$sign .= '^T'.$ort_datum.'|'.(($pdf_array['w_body'] / 2.0) + 10.0).'|'.($sign_y + 8.0).'|8|left|normal';
	$orderline=str_replace('^!sign', $sign, $orderline);
	// ...!adr
	$adr_pos=strpos($orderline, '^!adr');
	if($adr_pos !== false)
	{
		$adr_anrede		= '';
		$adr_name		= '';
		$adr_ansp		= '';
		$adr_ansp_zus	= '';
		$adr_strasse	= '';
		$adr_ort		= '';

		$end_pos=strpos($orderline, '^', $adr_pos+5);
		if($end_pos === false)
			$end_pos=strlen($orderline);
		$data_y=138.0;
		$data='';
		if(substr($orderline, $adr_pos+5, 1) == '|')
		{
			$absender=substr($orderline, $adr_pos+6, $end_pos-$adr_pos-6);
			if($absender && ($absender !== '0'))
				$data='^T'.$absender.'|&70.0|&'.$data_y.'|7|left|normal';
		}
		$adr_array=array();
		if(fxIsArray($rechnung_array))
		{
			if(strlen($rechnung_array['kto']))
			{
				$chk_arr=explode(',', $rechnung_array['kto']);
				foreach($chk_arr as $c)
				{
					$rad_arr=explode('_', $c);
					if($rad_arr[0] == 2113)
					{
						if(isset($GLOBALS['_preview_invoice']))
							$ktxt='(KTO rechnungsanschrift)<:nl:>Test';
						else
							$ktxt=db_value("SELECT kommunikation_txt FROM kommunikation WHERE kommunikation_id=".(int)$rad_arr[1]." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);

						if(strlen($ktxt))
						{
							$nltr=array("\r\n"=>'<:nl:>', "\r"=>'<:nl:>', "\n"=>'<:nl:>');
							$adr_array=explode('<:nl:>', strtr($ktxt, $nltr));
						}

						break;
					}
				}
			}

			if(!sizeof($adr_array))
			{
				$adr_cnt=0;
				// ...Anrede Auftraggeber
				if(strlen(trim($rechnung_array['anredetext'])))
				{
					$adr_array[$adr_cnt]=trim($rechnung_array['anredetext']);
					$adr_cnt++;
				}
				// ...Name Auftraggeber
				$adr_name=trim($rechnung_array['pname']);
				if(strlen(trim($rechnung_array['vorname'])))
					$adr_name=trim($rechnung_array['vorname']).' '.$adr_name;
				$adr_name_array=fxpdf_wraptext($pdf, $adr_name, 10.00, '', 230.00);
				if(fxIsArray($adr_name_array))
				{
					foreach($adr_name_array as $adr_name_line)
					{
						$adr_array[$adr_cnt]=$adr_name_line;
						$adr_cnt++;
					}
				}
				// ...Anrede+Name Ansprechpartner
				if(isset($GLOBALS['_preview_invoice_contact']))
				{
					$adr_array[$adr_cnt]=$GLOBALS['_preview_invoice_contact'];
					$adr_cnt++;
				}
				else
				{
					$ansp=(int)$rechnung_array['person_angelegt'];
					if($ansp)
					{
						$ansparr=db_values("SELECT anrede, perstitel, pname, vorname, name_zusatz FROM personen WHERE personen_id=".$ansp." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
						if(is_array($ansparr))
						{
							$adr_ansp=trim($ansparr['pname']);
							if(strlen(trim($ansparr['vorname'])))
								$adr_ansp=trim($ansparr['vorname']).' '.$adr_ansp;
							if($ansparr['perstitel'])
								$adr_ansp=meldung($ansparr['perstitel'], true).' '.$adr_ansp;
							if($ansparr['anrede'])
								$adr_ansp=meldung($ansparr['anrede'], true).' '.$adr_ansp;
							$adr_array[$adr_cnt]=$adr_ansp;
							$adr_cnt++;
							// ...Zusatz Ansprechpartner
							if(strlen(trim($ansparr['name_zusatz'])))
							{
								$adr_array[$adr_cnt]=trim($ansparr['name_zusatz']);
								$adr_cnt++;
							}
						}
					}
				}
				// ...Strasse
				$adr_array[$adr_cnt]=trim($rechnung_array['strasse']);
				$adr_cnt++;
				// ...LKZ + PLZ + Ort
				$adr_ort=trim($rechnung_array['ort']);
				if(strlen(trim($rechnung_array['plz'])))
				{
					$adr_plz=trim($rechnung_array['plz']);
					if(strlen(trim($rechnung_array['staat'])))
						$adr_plz=trim($rechnung_array['staat']).'-'.$adr_plz;
					$adr_ort=$adr_plz.' '.$adr_ort;
				}
				$adr_array[$adr_cnt]=$adr_ort;
				$adr_cnt++;
			}
		}

		if(is_array($adr_array))
		{
			$data_y += 10.0;
			foreach($adr_array as $adr_cnt => $adr_line)
			{
				if(($adr_cnt+1) == sizeof($adr_array))
					$data_y += 20.0;
				else
					$data_y += 12.0;

				$data .= '^T'.$adr_line.'|&70.0|&'.$data_y.'|10|left|normal';
			}
		}

		$data .= '^L&10.0|&289.0|&28.0|&289.0';
		$data .= '^L&10.0|&570.0|&28.0|&570.0';

		if($end_pos == strlen($orderline))
			$orderline=substr($orderline, 0, $adr_pos).$data;
		else
			$orderline=substr($orderline, 0, $adr_pos).$data.substr($orderline, end_pos);
	}
	// ...!date
	$now=fxNow();
	$orderline=str_replace('<!date>', get_entry($now, 'datum'), $orderline);
	// ...!time
	$orderline=str_replace('<!time>', get_entry($now, 'zeit'), $orderline);
	// ...!logo
	$orderline=str_replace('<!logo>', 'logo_m'.$GLOBALS['fxpglobals']['client'], $orderline);
	// ...!project
	if(isset($GLOBALS['_preview_subst']) && is_array($GLOBALS['_preview_subst']) && isset($GLOBALS['_preview_subst']['project']))
		$orderline=str_replace('<!project>', $GLOBALS['_preview_subst']['project'], $orderline);
	else if(strlen($txt_prj))
		$orderline=str_replace('<!project>', $txt_prj, $orderline);
	else
		$orderline=str_replace('<!project>', meldung(1077, true), $orderline);	// Alle Vorgänge
	// ...!customer
	if(isset($GLOBALS['_preview_subst']) && is_array($GLOBALS['_preview_subst']) && isset($GLOBALS['_preview_subst']['customer']))
		$orderline=str_replace('<!customer>', $GLOBALS['_preview_subst']['customer'], $orderline);
	else if(strlen($txt_cust))
		$orderline=str_replace('<!customer>', $txt_cust, $orderline);
	else
		$orderline=str_replace('<!customer>', meldung(1545, true), $orderline);		// Alle Auftraggeber
	// ...!name
	if(isset($GLOBALS['_preview_subst']) && is_array($GLOBALS['_preview_subst']) && isset($GLOBALS['_preview_subst']['name']))
		$orderline=str_replace('<!name>', $GLOBALS['_preview_subst']['name'], $orderline);
	else if(strlen($txt_pers))
		$orderline=str_replace('<!name>', $txt_pers, $orderline);
	else
		$orderline=str_replace('<!name>', meldung(1410, true), $orderline);		// Alle Personen
	// ...!timespan
	if(isset($GLOBALS['_preview_subst']) && is_array($GLOBALS['_preview_subst']) && isset($GLOBALS['_preview_subst']['timespan']))
		$orderline=str_replace('<!timespan>', $GLOBALS['_preview_subst']['timespan'], $orderline);
	else if(strlen($txt_timespan))
		$orderline=str_replace('<!timespan>', $txt_timespan, $orderline);
	else
		$orderline=str_replace('<!timespan>', meldung(1678, true), $orderline);		// Unbegrenzt

	return $orderline;
}

/**
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  DPDF_GET_ZINFO                                                           +
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf - Mandatory parameter: ???PARAMETER???
 */
function dpdf_get_zinfo(&$pdf)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Skalierungsfaktor + Zeichensatzgrösse ermitteln
	dpdf_set_fac($GLOBALS['pdf_array']['fac']);
	if($GLOBALS['pdf_get_zinfo_debug'])
		echo('<B>Fontsize (Spalteneinträge):</B> '.$GLOBALS['pdf_array']['fs'].', <B>Fontsize (Gesamtüberschrift):</B> '.$GLOBALS['pdf_array']['fshl'].', <B>Fontsize (Tabellenüberschrift):</B> '.$GLOBALS['pdf_array']['fsh'].', <B>Skalierungsfaktor:</B> '.$GLOBALS['pdf_array']['fac'].', <B>Cellspacing:</B> X'.$GLOBALS['pdf_array']['xd'].'/Y'.$GLOBALS['pdf_array']['yd'].'<BR>');

	// Spaltenbreite + Zeilenanzahl
	if(fxIsArray($GLOBALS['print_array'][0]))
	{
		dpdf_get_col_width($pdf);
//fxDebug($GLOBALS['pdf_col_width'], 'pdf_col_width ('.__LINE__.')');
		if($GLOBALS['pdf_samewidth'] && fxIsArray($GLOBALS['pdf_col_width']))	// Gleiche Spaltenbreiten
		{
			// Größte Spaltenbreite ermitteln...
			$maxwidth=0.00;
			foreach($GLOBALS['pdf_col_width'] as $clcnt => $clwd)
			{
				if($clwd > $maxwidth)
					$maxwidth=$clwd;
			}
			// ...und wieder zurückschreiben
			foreach($GLOBALS['pdf_col_width'] as $clcnt => $clwd)
				$GLOBALS['pdf_col_width'][$clcnt]=$maxwidth;
		}

		if($GLOBALS['pdf_sameheight'] && fxIsArray($GLOBALS['pdf_lines']))	// Gleiche Zeilenanzahl
		{
			// Größte Zeilenanzahl ermitteln...
			$maxheight=0;
			foreach($GLOBALS['pdf_lines'] as $lncnt => $lnht)
			{
				if($lnht > $maxheight)
					$maxheight=$lnht;
			}
			// ...und wieder zurückschreiben
			foreach($GLOBALS['pdf_lines'] as $lncnt => $lnht)
				$GLOBALS['pdf_lines'][$lncnt]=$maxheight;
		}

		if($GLOBALS['_POST']['tr'] == 84)	// Projektstrukturplan
		{
			$GLOBALS['pdf_subwidth']	= $GLOBALS['pdf_array']['fs'];
			$GLOBALS['pdf_subheight']	= $GLOBALS['pdf_array']['fs'];
			$GLOBALS['pdf_subwidth2']	= $GLOBALS['pdf_subwidth']/2.0;
			$GLOBALS['pdf_subheight2']	= $GLOBALS['pdf_subheight']/2.0;
			foreach($GLOBALS['pdf_col_width'] as $clcnt => $clwd)
				$GLOBALS['pdf_col_width'][$clcnt]=$clwd+$GLOBALS['pdf_subwidth'];
			foreach($GLOBALS['pdf_lines'] as $lncnt => $lnht)
				$GLOBALS['pdf_lines'][$lncnt] += 1.5;
		}

		// Anzeige anpassen auf max. eine Seitenbreite?
		if($GLOBALS['pdf_array']['fit'])
		{
			// Originalbreite sortiert von klein zu groß
			$GLOBALS['pdf_socol_width']=$GLOBALS['pdf_col_width'];
			asort($GLOBALS['pdf_socol_width'], SORT_NUMERIC);
//fxDebug($GLOBALS['pdf_socol_width'], __LINE__.' - $pdf_socol_width');

			// Gesamtbreite
			$gb=0.00;
			$mb=-1;
			foreach($GLOBALS['pdf_col_width'] as $spn => $spw)
			{
				$gb += $spw;
				if(($mb < 0) || ($spw > $GLOBALS['pdf_col_width'][$mb]))
					$mb=$spn;
			}
//echo('<b>Gesamtbreite:</b> '.$gb.' - <b>Seitenbreite:</b> '.$GLOBALS['pdf_array']['w_body'].' - <b>Max. Spalte:</b> '.$mb.' (= '.$GLOBALS['pdf_col_width'][$mb].')<br />');

			if($gb != $GLOBALS['pdf_array']['w_body'])
			{
				// Gesamtbreite < Anzeigebreite, dann breiteste Spalte verbreitern, sodass ganze Seitenbreite ausgefüllt wird
				if($gb < $GLOBALS['pdf_array']['w_body'])
				{
					$GLOBALS['pdf_col_width'][$mb] += ($GLOBALS['pdf_array']['w_body'] - 2.00 - $gb); // 2.00 Luft, damit Seite zu 100% passt
				}
				// Gesamtbreite > Anzeigebreite, dann Faktor anpassen
				else
				{
					$nfac=($GLOBALS['pdf_array']['w_body'] - 2.00)/$gb; // 2.00 Luft, damit Seite zu 100% passt
//echo('<B>Alter Faktor:</B> '.$GLOBALS['pdf_array']['fac'].' - <B>Neuer Faktor:</B> '.$nfac.'<BR>');

					// Zeichengrösse anpassen
					$GLOBALS['pdf_array']['xd']	=  3.0*$nfac;	// X-Distanz in Spalten (CELLSPACING)
//					$GLOBALS['pdf_array']['yd']	=  3.0*$nfac;	// Y-Distanz in Spalten (CELLSPACING)
					$GLOBALS['pdf_subwidth']	= $GLOBALS['pdf_subwidth']*$nfac;
//					$GLOBALS['pdf_subheight']	= $GLOBALS['pdf_subheight']*$nfac;

					// Spaltenbreite anpassen
					$d=$gb-$GLOBALS['pdf_array']['w_body'] + 2.00;	// 2.00 Luft, damit Seite zu 100% passt
					$ncd=array();
					$ncw=$GLOBALS['pdf_col_width'];
					// ...Breiteste Spalte ermitteln, welche noch nicht bearbeitet wurde
					while(($d > 0.00) && (sizeof($ncd) < sizeof($ncw)))
					{
						$bs=-1;
						foreach($ncw as $spn => $spw)
						{
							if(!isset($ncd[$spn]))
							{
								if(($bs < 0) || ($spw > $ncw[$bs]))
									$bs=$spn;
							}
						}
						if($bs >= 0)
						{
							$n=$GLOBALS['pdf_col_width'][$bs]*0.25;
							$m=max($n, $ncw[$bs]-$n);
							if($ncw[$bs]-$d >= $m)
							{
								$ncw[$bs] -= $d;
								$d=0.00;
								break;
							}
							$d -= $ncw[$bs]-$m;
							$ncw[$bs]=$m;
							$ncd[$bs]=true;

						}
						else
							break;
					}
					if($d == 0.00)
						$GLOBALS['pdf_col_width']=$ncw;
					else
					{
						foreach($GLOBALS['pdf_col_width'] as $spn => $spw)
							$GLOBALS['pdf_col_width'][$spn]=$spw*$nfac;
					}
//fxDebug($GLOBALS['pdf_col_width'], __LINE__.' - $pdf_col_width');

					if(!$GLOBALS['pdf_samewidth'])	// Nur falls KEINE gleichen Spaltenbreiten


					{
						// Minumimspaltenbreite berücksichtigen
						$msw=fxpdf_strlen($pdf, 'M', $GLOBALS['pdf_array']['fsh'], 'B') + $GLOBALS['pdf_array']['xd']*5.0;
//fxDebug($msw, __LINE__.' - $msw: $mb='.$mb);
						if($mb >= 0)
						{
							foreach($GLOBALS['pdf_col_width'] as $spn => $spw)
							{
//fxDebug($spw, __LINE__.' - $spw: $spn='.$spn.', $mb='.$mb);
								if($spw < $msw)
								{
									$dsw=$msw-$spw;
									$GLOBALS['pdf_col_width'][$spn]=$msw;
									$GLOBALS['pdf_col_width'][$mb]=max($msw,$GLOBALS['pdf_col_width'][$mb]-$dsw);

									// Neue Maximalspalte suchen
									$nmb=-1;
									foreach($GLOBALS['pdf_col_width'] as $nspn => $nspw)
									{
										if(($nmb < 0) || ($nspw > $GLOBALS['pdf_col_width'][$nmb]))
											$nmb=$nspn;
									}
									$mb=$nmb;
								}
							}
						}
//fxDebug($GLOBALS['pdf_col_width'], __LINE__.' - $pdf_col_width: $mb='.$mb);


						// Evtl. Spalten schrittweise wieder vergrößern?
						$cwp=0.8;
						if($mb >= 0)




						{
							$zsw=fxpdf_strlen($pdf, 'M', $GLOBALS['pdf_array']['fsh']);
//fxDebug($zsw, __LINE__.' - $zsw');
							$wda=array();
							$wsa=array();
							while(true)
							{
								$mnn=-1;
								$mxn=-1;
								foreach($GLOBALS['pdf_col_width'] as $spn => $spw)
								{
									if(!isset($wda[$spn]))
									{
										if(($mnn < 0) || ($spw <= $GLOBALS['pdf_col_width'][$mnn]))
											$mnn=$spn;
										if(($mxn < 0) || ($spw > $GLOBALS['pdf_col_width'][$mxn]))
											$mxn=$spn;
									}
								}
//fxDebug($GLOBALS['pdf_col_width'], __LINE__.' - $pdf_col_width: $mnn='.$mnn.', $mxn='.$mxn);
								if(($mnn >= 0) && ($mxn >= 0) && ($mxn != $mnn) && ($GLOBALS['pdf_col_width'][$mnn]+$zsw < $GLOBALS['pdf_col_width'][$mxn]*$cwp) && !isset($wsa[$mnn]))






								{
									$dsw=min($zsw, $GLOBALS['pdf_socol_width'][$mnn]-$GLOBALS['pdf_col_width'][$mnn]);
//fxDebug($dsw, __LINE__.' - $dsw: $mnn='.$mnn.', $mxn='.$mxn);
									if($dsw <= 0.0)
										$wda[$mnn]=$GLOBALS['pdf_col_width'][$mnn];
									else
									{
										$GLOBALS['pdf_col_width'][$mnn] += $dsw;
										$GLOBALS['pdf_col_width'][$mxn] -= $dsw;
										$wsa[$mxn]=$GLOBALS['pdf_col_width'][$mxn];
									}
								}
								else

									break;
							}
//fxDebug($wda, __LINE__.' - $wda'); fxDebug($wsa, __LINE__.' - $wsa');


						}
					}
//fxDebug($GLOBALS['pdf_col_width'], 'pdf_col_width ('.__LINE__.')');

					// Überschriften anpassen
					foreach($GLOBALS['print_array'][-1] as $sn => $txt)
					{
						$w=fxpdf_strlen($pdf, $txt, $GLOBALS['pdf_array']['fsh'], 'B') + $GLOBALS['pdf_array']['xd']*2.5;
						if(isset($GLOBALS['pdf_col_width'][$sn]) && ($w > $GLOBALS['pdf_col_width'][$sn]))
						{
							$ta=fxpdf_wraptext($pdf, $txt, $GLOBALS['pdf_array']['fsh'], 'B', $GLOBALS['pdf_col_width'][$sn]-5*$GLOBALS['pdf_array']['xd']);
							$GLOBALS['print_array'][-1][$sn]=$ta[0];
						}
					}

					// Einträge anpassen
					$aa=array();
//fxDebug($GLOBALS['pdf_columns'],'$pdf_columns');
					foreach($GLOBALS['pdf_columns'] as $zn => $zeile)
					{
						foreach($zeile as $sn => $spalte)
						{
							foreach($spalte as $szn => $subzeile)
							{
								foreach($subzeile as $ssn => $subspalte)
								{
									$cs=(int)$GLOBALS['print_array'][$zn][$sn]['cs'];
									$w=$GLOBALS['pdf_array']['xd']+$GLOBALS['pdf_columns'][$zn][$sn][$szn][$ssn]['xc']+$GLOBALS['pdf_columns'][$zn][$sn][$szn][$ssn]['w']+$GLOBALS['pdf_array']['xd'];
									$m=0.00;
									if(isset($GLOBALS['pdf_col_width'][$sn]))
										$m=$GLOBALS['pdf_col_width'][$sn];
									if($cs > 1)
									{
										// Colspanbreite ermitteln
										$cc=1;
										foreach($GLOBALS['pdf_col_width'] as $cspalte => $cwidth)
										{
											if($cspalte <= $sn)
												continue;
											if($cc < $cs)
												$m += $cwidth;
											else
												break;
											$cc++;
										}
									}
									if($w > $m)
									{
										if($subspalte['h'])
										{
											$GLOBALS['pdf_columns'][$zn][$sn][$szn][$ssn]['xc'] *= $nfac;
											$GLOBALS['pdf_columns'][$zn][$sn][$szn][$ssn]['w'] *= $nfac;
											$GLOBALS['pdf_columns'][$zn][$sn][$szn][$ssn]['h'] *= $nfac;
										}
										else
										{
											$ta=fxpdf_wraptext($pdf, $GLOBALS['pdf_columns'][$zn][$sn][$szn][$ssn]['txt'], $GLOBALS['pdf_array']['fs'], '', $m-6*$GLOBALS['pdf_array']['xd']-$GLOBALS['pdf_columns'][$zn][$sn][$szn][$ssn]['xc']);
//fxDebug($ta,'$ta: (WRAP) - $zn='.$zn.', $sn='.$sn);
											$aa[]=array('zn'=>$zn, 'sn'=>$sn, 'szn'=>$szn, 'ssn'=>$ssn, 'ta'=>$ta);
										}
									}
									else if(isset($GLOBALS['pdf_columns'][$zn][$sn][$szn][$ssn]['txt']))
									{
										$aa[]=array('zn'=>$zn, 'sn'=>$sn, 'szn'=>$szn, 'ssn'=>$ssn, 'ta'=>array(0=>$GLOBALS['pdf_columns'][$zn][$sn][$szn][$ssn]['txt']));
//fxDebug($GLOBALS['pdf_columns'][$zn][$sn][$szn][$ssn]['txt'],'$aa[\'ta\']: $zn='.$zn.', $sn='.$sn);
									}
								}
							}
						}
					}
//fxDebug($aa, '$aa');

					if(fxIsArray($aa))
					{
						$aaa=array();
						$ala=array();
						foreach($aa as $a)
						{
							$lnc=1;
							$zn=$a['zn'];
							$sn=$a['sn'];
							$id=$zn.'|'.$sn;
							if(!isset($aaa[$id]))
								$aaa[$id]=0;
							$szn=$a['szn']+$aaa[$id];
							$ssn=$a['ssn'];
							$GLOBALS['pdf_columns'][$zn][$sn][$szn][$ssn]['txt']=$a['ta'][0];
							foreach($a['ta'] as $na => $nt)
							{
								if($na > 0)
								{
									if(fxIsArray($GLOBALS['pdf_columns'][$zn][$sn][$szn+$na]))
									{
										$soca=1+sizeof($GLOBALS['pdf_columns'][$zn][$sn]);
										for($ni=$soca; $ni>$szn+$na; $ni--)
											$GLOBALS['pdf_columns'][$zn][$sn][$ni]=$GLOBALS['pdf_columns'][$zn][$sn][$ni-1];
									}
									$GLOBALS['pdf_columns'][$zn][$sn][$szn+$na]=array(0 => $GLOBALS['pdf_columns'][$zn][$sn][$szn][$ssn]);
									$GLOBALS['pdf_columns'][$zn][$sn][$szn+$na][0]['txt']=$nt;
									$aaa[$id]++;
									$lnc++;
								}
							}
							if(!isset($ala[$zn]))
								$ala[$zn]=array('c'=>array(), 'l'=>0);
							if(!isset($ala[$zn]['c'][$sn]))
								$ala[$zn]['c'][$sn]=0;
							$ala[$zn]['c'][$sn] += $lnc;
							$ala[$zn]['l']=max($ala[$zn]['l'], $ala[$zn]['c'][$sn]);
						}
//fxDebug($ala,'$ala');
						if(sizeof($ala))
						{
							foreach($ala as $zn => $aza)
							{
								if(!isset($GLOBALS['pdf_lines'][$zn]) || ($aza['l'] > $GLOBALS['pdf_lines'][$zn]))
									$GLOBALS['pdf_lines'][$zn]=$aza['l'];
							}
						}
					}
				}
			}
		}
	}

	// Summenzeile
	if($GLOBALS['pdf_array']['h_sum'])
		$GLOBALS['pdf_array']['h_sum']=$GLOBALS['pdf_array']['yd']*2.00 + $GLOBALS['pdf_array']['fs'];

	// Rechnungskopf + -fuss
	$GLOBALS['pdf_array']['h_chead']=dpdf_chead($pdf, 0, 0.0);
	$GLOBALS['pdf_array']['h_cfoot']=dpdf_cfoot($pdf, 0, 0.0);

	// Seitenanzahl ermitteln
	$pages=1;
	if(fxIsArray($GLOBALS['pdf_lines']))
	{
		$ml=0;
		$spl=sizeof($GLOBALS['pdf_lines']);

		$hb=$GLOBALS['pdf_array']['height']-$GLOBALS['pdf_array']['tmargin']-$GLOBALS['pdf_array']['h_pfoot']-$GLOBALS['pdf_array']['bmargin']-$GLOBALS['pdf_array']['h_sum'];
		if($GLOBALS['config'] == 2)		// Rechnung		-- (Überschriften) 3.0 + (Spielraum) 3.0=6.0
			$hb -= ($GLOBALS['pdf_array']['fsh'] + 6.0*$GLOBALS['pdf_array']['yd']);
		else							// Reports		-- (Gesamtüberschrift) 4.0 + (Überschriften) 3.0 + (Spielraum) 3.0=10.0
			$hb -= ($GLOBALS['pdf_array']['fshl'] + $GLOBALS['pdf_array']['fsh'] + 10.0*$GLOBALS['pdf_array']['yd']);
		$hh=$hb-$GLOBALS['pdf_array']['h_head']-$GLOBALS['pdf_array']['h_chead']-$GLOBALS['pdf_array']['h_cfoot']-$GLOBALS['pdf_array']['h_foot']-$GLOBALS['pdf_array']['h_sum'];
		if($GLOBALS['pdf_get_zinfo_debug'])
			echo('<B>Gesamtzeilen:</B> '.$spl.', <B>hh:</B> '.$hh.', <B>hb:</B> '.$hb.'<BR>');

		// Seiten zählen
		$sh=0;
		$lo=1;
		$pc=0;
		while($ml < $spl)
		{
//$pc++; if($pc>100){echo('pc>100 - Emergency Break<br />'); break;}
			$zz=$GLOBALS['pdf_lines'][$ml]+1-$lo;
			$zh=2.0*$GLOBALS['pdf_array']['yd'] + $zz*$GLOBALS['pdf_array']['fs'];
			$maxh=$hb;
			if($pages == 1)
				$maxh=$hh;
			if($GLOBALS['pdf_get_zinfo_debug'])
				echo('<b>akt. Zeilenzähler:</b> '.$ml.', <b>akt. Seite:</b> '.$pages.', <b>Zeilen:</b> '.$zz.', <b>Zeilenoffset:</b> '.$lo.', <b>bish. Gesamthöhe dieser Seite:</b> '.$sh.' + <b>akt. Zeilenhöhe:</b> '.$zh.'=<b>err. neue Gesamthöhe:</b> '.($sh+$zh).', <b>max. Seitenhöhe:</b> '.$maxh.'<br />');

			if(($sh + $zh) < $maxh)
			{
				$sh += $zh;
				$ml++;
				$lo=1;
			}
			else
			{
				if($GLOBALS['pdf_get_zinfo_debug'])
					echo('-&gt; <b>akt. Zeilenzähler:</b> '.$ml.', <b>akt. Seite:</b> '.$pages.', <b>Zeilenoffset:</b> '.$lo.'<br />');
				$ls=$lo;
				$lo += floor(($maxh - $sh - 2.0*$GLOBALS['pdf_array']['yd']) / $GLOBALS['pdf_array']['fs']);
				$pages++;
				$sh=0;
			}
		}
	}

	$GLOBALS['pdf_array']['pages']=$pages; // Seitenanzahl
	if($GLOBALS['pdf_get_zinfo_debug'])
		echo('<B>Seitenanzahl:</B> '.$GLOBALS['pdf_array']['pages'].'<BR>');
}

/**
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  DPDF_GET_COL_WIDTH                                                       +
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf - Mandatory parameter: ???PARAMETER???
 */
function dpdf_get_col_width(&$pdf)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Summenspalten
	$sum=array();

	// Alle Spaltenbreiten der Überschrift, CSV-Spaltennummern und Summenzeilen ermitteln
	$sc='';
	foreach($GLOBALS['print_array'][-1] as $spalte => $value)
	{
		$GLOBALS['pdf_col_width'][$spalte]=fxpdf_strlen($pdf, $value, $GLOBALS['pdf_array']['fsh'], 'B') + $GLOBALS['pdf_array']['xd']*2.5;
		if($GLOBALS['pdf_get_col_width_debug'] == 1)
			echo('<div style="background:#eeeeff;padding:2px;border-top:1px solid black;"><i>(L'.__LINE__.')</i> <b>Headline</b> ['.$spalte.']: value='.$value.' -&gt; width='.$GLOBALS['pdf_col_width'][$spalte].'</div>');

		// Summe
		if($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$spalte]['anzeigeart'] == 14)
		{
			$GLOBALS['pdf_array']['h_sum']=1;

			$sum[$spalte]=array('val'=>0.00, 'show'=>"", 'width'=>0.00);

			// Alle Zeilen durchgehen
			foreach($GLOBALS['print_array'] as $al => $zeile)
			{
				// Alle Überschriften überspringen
				if($al < 0)
					continue;

				if(is_array($zeile[$spalte]) && strlen($zeile[$spalte]['fw']))
				{
					$ffc=substr($zeile[$spalte]['fw'], 0, 1);
					$flc=substr($zeile[$spalte]['fw'], -1);
					if(($ffc != '~') && ($flc != '^') && ($flc != '|') && ($flc != '~') && ($flc != '>'))
					{
						$sfw=strip_tags($zeile[$spalte]['fw']);
						$sum[$spalte]['val'] += get_double($sfw);
						if($GLOBALS['pdf_get_col_width_debug'] == 1)
							echo('<div style="background:#ffeeee;padding:2px;border-top:1px solid black;"><i>(L'.__LINE__.')</i> <b>Sum</b> ['.$spalte.']: fw='.fxHtmlEncode($zeile[$spalte]['fw']).' -&gt; sfw='.fxHtmlEncode($sfw).' -&gt; val='.$sum[$spalte]['val'].'</div>');
					}
				}
			}

			$lv=get_entry($sum[$spalte]['val'], $GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$spalte], false);
			$lu='';
			if(!$GLOBALS['no_waehrung'])
			{
				if($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$spalte]['id'] == 71)			// MwSt.-Satz
					$lu=' %';
				else if(strlen($GLOBALS['waehrung']) && (($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$spalte]['feldtyp'] == 11) || ($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$spalte]['feldtyp'] == 16) || ($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$spalte]['feldtyp'] == 17)))	// Waehrung
					$lu=' '.$GLOBALS['waehrung'];
			}

			$sum[$spalte]['show']	= $lv.$lu;
			$sum[$spalte]['width']	= fxpdf_strlen($pdf, $sum[$spalte]['show'], $GLOBALS['pdf_array']['fs'], 'B') + $GLOBALS['pdf_array']['xd']*2.5;

			if($sum[$spalte]['width'] > $GLOBALS['pdf_col_width'][$spalte])
			{
				$GLOBALS['pdf_col_width'][$spalte]=$sum[$spalte]['width'];
				if($GLOBALS['pdf_get_col_width_debug'] == 1)
					echo('<div style="background:#eeffee;padding:2px;border-top:1px solid black;"><i>(L'.__LINE__.')</i> <b>Sum</b> ['.$spalte.']: val='.$sum[$spalte]['val'].', show='.$sum[$spalte]['show'].' -&gt; width='.$GLOBALS['pdf_col_width'][$spalte].'</div>');
			}
		}

		if(strlen($sc))
			$sc .= ',';
		$sc .= $spalte;
	}

	// CSV-Spaltennummern umdrehen und zu einem Array konvertieren
	$scr='';
	$asz='';
	for($scc=0; $scc<=strlen($sc); $scc++)
	{
		if($scc == strlen($sc))
			$aktz=',';
		else
			$aktz=substr($sc, $scc, 1);
		if($aktz != ',')
			$asz .= $aktz;
		else if(strlen($asz))
		{
			if(strlen($scr))
				$scr=$asz.','.$scr;
			else
				$scr=$asz;
			$asz='';
		}
	}
	$sc_array	= explode(',', $sc);
	$scr_array	= explode(',', $scr);

	$csarr=array();

	// Alle Zeilen durchgehen
	$brtr1=array('|z'=>'|Z');
	foreach($GLOBALS['print_array'] as $al => $zeile)
	{
		// Alle Überschriften überspringen
		if($al < 0)
			continue;

		// Alle Spalten durchgehen
		foreach($scr_array as $sz => $spalte)
		{
			if(is_array($GLOBALS['print_array'][$al][$spalte]))
			{
				$GLOBALS['print_array'][$al][$spalte]['show']=strtr($GLOBALS['print_array'][$al][$spalte]['show'],$brtr1);

				$endkom=false;
				// Übergeordnete Zeicheninformation
				if($GLOBALS['print_array'][$al][$spalte]['fwp'])
				{
					$fwp=strtoupper($GLOBALS['print_array'][$al][$spalte]['fwp']);
					if($GLOBALS['pdf_get_col_width_debug'] > 1)
						echo('<div style="background:#eeeeee;padding:2px;border-top:1px solid black;"><i>(L'.__LINE__.')</i> <b>fwp:</b> '.fxHtmlEncode($fwp).'</div>');
					// BGCOLOR
					$clp=strpos($fwp, '^COL_');
					if($clp !== false)
					{
						$cle=strpos($fwp, '"', $clp);
						if($cle === false)
							$bgc=trim(substr($fwp, $clp+5));
						else
							$bgc=trim(substr($fwp, $clp+5, $cle-$clp-5));
						if(strlen($bgc) == 6)
							$GLOBALS['print_array'][$al][$spalte]['bcol']=$bgc;
					}
					else
					{
						$clp=strpos($fwp, '^CLASS_');
						if($clp !== false)
						{
							$cle=strpos($fwp, '"', $clp);
							if($cle === false)
								$cls=strtolower(substr($fwp, $clp+7));
							else
								$cls=strtolower(substr($fwp, $clp+7, $cle-$clp-7));
							$csp=strpos($cls, ' ');
							if($csp !== false)
								$cls=substr($cls, 0, $csp);
//echo('<div style="background:#ffffff;padding:2px;"><font color="#008800">'.__LINE__.':</font> - <b>cls:</b> '.fxHtmlEncode($cls).'</div>');
							$GLOBALS['print_array'][$al][$spalte]['class']=$cls;
							$clscol=$GLOBALS['bstyle'][$cls];
							if(strlen($clscol))
								$GLOBALS['print_array'][$al][$spalte]['bcol']=$clscol;
							$clscol=$GLOBALS['cstyle'][$cls];
							if(strlen($clscol))
								$GLOBALS['print_array'][$al][$spalte]['col']=$clscol;
						}
						else
						{
							$clp=strpos($fwp, '^ID_');
							if($clp !== false)
							{
								$cle=strpos($fwp, '"', $clp);
								if($cle === false)
									$GLOBALS['print_array'][$al][$spalte]['bcol']=$GLOBALS['bstyle'][strtolower(substr($fwp, $clp+4))];
								else
									$GLOBALS['print_array'][$al][$spalte]['bcol']=$GLOBALS['bstyle'][strtolower(substr($fwp, $clp+4, $cle-$clp-4))];
							}
						}
					}
					if($GLOBALS['print_array'][$al][$spalte]['bcol'] == '?')
						unset($GLOBALS['print_array'][$al][$spalte]['bcol']);
					// FRAMECOLOR
					$frp=strpos($fwp, 'FRCOL=');
					if($frp !== false)
					{
						if(substr($fwp, $frp+6, 1) == '"')
							$GLOBALS['print_array'][$al][$spalte]['fcol']=substr($fwp, $frp+7, 6);
						else
							$GLOBALS['print_array'][$al][$spalte]['fcol']=substr($fwp, $frp+6, 6);
					}
					// TOOLTIP
					$ttp=strpos($fwp,'TOOLTIP=');
					if($ttp !== false)
					{
						$p=$ttp+8;
						if(substr($fwp,$p,1) == '"')
							$p++;
						$e=strpos($fwp,'"',$p);
						if($e === false)
							$tooltip=substr($GLOBALS['print_array'][$al][$spalte]['fwp'],$p);
						else
							$tooltip=substr($GLOBALS['print_array'][$al][$spalte]['fwp'],$p,$e-$p);
						$dach=strpos($tooltip,'^');
						if($dach !== false)
							$GLOBALS['pdf_tooltips'][substr($tooltip,0,$dach)]=substr($tooltip,$dach+1);
						else
							$GLOBALS['pdf_tooltips']['^'.substr('0000'.$al,-4).substr('0000'.$spalte,-4)]=$tooltip;
					}
					// COLSPAN
					$csp=strpos($fwp, 'COLSPAN=');
					if($csp !== false)
					{
						if(substr($fwp, $csp+8, 1) == '"')
							$csc=(int)substr($fwp, $csp+9);
						else
							$csc=(int)substr($fwp, $csp+8);
						if($csc > 1)
						{
							$GLOBALS['print_array'][$al][$spalte]['cs']=$csc;
							foreach($sc_array as $cspalte)
							{
								if($cspalte <= $spalte)
									continue;
								if($csc > 1)
									$GLOBALS['print_array'][$al][$cspalte]['bcs']=$spalte.'cs';
								else
									break;
								--$csc;
							}
						}
					}
					// ALIGN
					$alp=strpos($fwp, 'ALIGN=');
					if($alp !== false)
					{
						if(substr($fwp, $alp+6, 1) == '"')
							$alz=substr($fwp, $alp+7, 1);
						else
							$alz=substr($fwp, $alp+6, 1);
						if($alz == 'L')
							$GLOBALS['print_array'][$al][$spalte]['align']='left';
						else if($alz == 'R')
							$GLOBALS['print_array'][$al][$spalte]['align']='right';
						else
							$GLOBALS['print_array'][$al][$spalte]['align']='center';
					}
					// ENDKOM
					$ekp=strpos($fwp, '-->');
					if($ekp !== false)
						$endkom=true;
				}

				// Aktuelle Zeicheninformation
				if(substr($GLOBALS['print_array'][$al][$spalte]['show'],-2) == '|Z')
				{
					$GLOBALS['print_array'][$al][$spalte]['show']=substr($GLOBALS['print_array'][$al][$spalte]['show'], 0, -2);
					$GLOBALS['print_array'][$al][$spalte]['bcol']='eeeeee';
				}
				foreach($scr_array as $tspalte)
				{
					if(($tspalte > $spalte) && $GLOBALS['print_array'][$al][$spalte]['bcol'] && !$GLOBALS['print_array'][$al][$tspalte]['bcol'])
						$GLOBALS['print_array'][$al][$tspalte]['bcol']=$GLOBALS['print_array'][$al][$spalte]['bcol'];
				}

				$GLOBALS['print_array'][$al][$spalte]['lines']=dpdfExtractText($al,$spalte);
				if($GLOBALS['pdf_get_col_width_debug'] > 1)
				{
					echo('<div style="padding-left:24px;border-radius:4px;padding:2px;margin-bottom:1px;width:1024px;background:#e8e8e8;"><i>(L'.__LINE__.')</i> <b>lines</b> - $al='.$al.', $spalte='.$spalte.'<br />');
					fxDebug($GLOBALS['print_array'][$al][$spalte]['lines']);
					echo('</div>');
				}

				// Max Zeichenanzahl pro Zeile
				if(sizeof($GLOBALS['print_array'][$al][$spalte]['lines']))
				{
					$pal=$GLOBALS['print_array'][$al][$spalte]['lines'];

					$zn=1;
					$maxzl=(int)$GLOBALS['pdf_array']['maxlen'];
					if(!$maxzl)
						$maxzl=1024;
					else
						$maxzl=max(10,$maxzl);






					unset($GLOBALS['print_array'][$al][$spalte]['lines']);
					foreach($pal as $azeile)
					{
						$zeile=$azeile['txt'];

						// Grafiken und Colspan-Texte nicht beschränken
						$pxpos=strpos($zeile, '{PX:');
						if(($pxpos !== false) || ($GLOBALS['print_array'][$al][$spalte]['cs'] > 1))
						{
							$GLOBALS['print_array'][$al][$spalte]['lines'][$zn]=$azeile;
							$zn++;
							continue;
						}

						while(true)
						{







							$zl=strlen($zeile);
							if($zl <= $maxzl)
							{
								$GLOBALS['print_array'][$al][$spalte]['lines'][$zn]=$azeile;
								$zn++;
								break;
							}
							else
							{
								$zlpos=$maxzl;
								while($zlpos && (substr($zeile, $zlpos, 1) != ' '))
									--$zlpos;
								if($zlpos)
								{
									$GLOBALS['print_array'][$al][$spalte]['lines'][$zn]=azeile;
									$GLOBALS['print_array'][$al][$spalte]['lines'][$zn]['txt']=substr($zeile,0,$zlpos);
									$zeile=substr($zeile,$zlpos+1);
									$zn++;
								}
								else
								{
									$GLOBALS['print_array'][$al][$spalte]['lines'][$zn]=$azeile;
									$GLOBALS['print_array'][$al][$spalte]['lines'][$zn]['txt']=substr($zeile,0,$maxzl);
									$zeile=substr($zeile,$maxzl);
									$zn++;
								}
							}
						}
					}
					--$zn;
					if($zn > $GLOBALS['pdf_lines'][$al])
						$GLOBALS['pdf_lines'][$al]=$zn;
				}

				// Grafiken suchen
				if(is_array($GLOBALS['print_array'][$al][$spalte]['lines']))
				{
					$leadspace='';
					foreach($GLOBALS['print_array'][$al][$spalte]['lines'] as $zn => $azeile)
					{
						$zeile=$azeile['txt'];
//echo('<B>Zeile '.$zn.':</B> "'.fxHtmlEncode($zeile).'"<BR>');
						$zsz=0;
						$ssp=0;
						$bar=0;
						$stxc=0.00;	// Start-X-Koordinate
						if(strlen($zeile))
						{
							$pxex=explode('{PX:', $zeile);
							if($GLOBALS['pdf_grafik_debug'])
								fxDebug($pxex);
							foreach($pxex as $pxval)
							{
								if(!strlen($pxval))
									continue;

								$pxfc=substr($pxval, 0, 1);
								if(($pxfc != '@') && ($pxfc != '"'))
								{
									if(strlen($leadspace))
										$pxval=$leadspace.trim($pxval);
									else
										$pxval=$leadspace.$pxval;
//echo('<hr>['.$al.']['.$spalte.']['.$zn.']['.$zsz.'][txt]:<br />'.$pxval.'<br />');
									$sw=fxpdf_strlen($pdf, $pxval, $azeile['fs'], $azeile['md']);
									if($GLOBALS['pdf_get_col_width_debug'] == 1)
										echo('<div style="background:#ffffff;padding:2px;border-top:1px solid black;"><i>(L'.__LINE__.')</i> <b>$pxval</b> ['.fxHtmlEncode($pxval).'] -&gt; width='.$sw.'</div>');
									$GLOBALS['pdf_columns'][$al][$spalte][$zn][$zsz]=$azeile;
									$GLOBALS['pdf_columns'][$al][$spalte][$zn][$zsz]['txt']=$pxval;
									$GLOBALS['pdf_columns'][$al][$spalte][$zn][$zsz]['xc']=$stxc;
									$GLOBALS['pdf_columns'][$al][$spalte][$zn][$zsz]['w']=$sw;
									$stxc += $sw;
								}
								else
								{
									// Grafik
									$gre=strpos($pxval, '}');
									$image=substr($pxval, 0, $gre);
									$bild	= substr($image, 1+strpos($image, '@'));
									$hoehe	= (int)substr($image, 2+strpos($image, '"h'));
									if(!$hoehe)
										$hoehe=$GLOBALS['pdf_array']['fs']+$GLOBALS['pdf_array']['yd'];
									else
										$hoehe=$hoehe*0.80*$GLOBALS['pdf_array']['fac'];
									if($hoehe > $GLOBALS['pdf_lineheight'][$al])
										$GLOBALS['pdf_lineheight'][$al]=$hoehe;
									$breite	= (int)substr($image, 2+strpos($image, '"w'));
									if(!$breite)
										$breite=$hoehe;
									else
										$breite=$breite*0.80*$GLOBALS['pdf_array']['fac'];
									if($GLOBALS['pdf_grafik_debug'])
										echo(__LINE__.': -&gt; <b>IMAGE:</b> "'.fxHtmlEncode($image).'", <b>$bild:</b> "'.fxHtmlEncode($bild).'", <b>$breite:</b> "'.$breite.'", <b>$hoehe:</b> "'.$hoehe.'"<br />');
									$GLOBALS['pdf_columns'][$al][$spalte][$zn][$zsz]=$azeile;
									$GLOBALS['pdf_columns'][$al][$spalte][$zn][$zsz]['txt']=$bild;
									$GLOBALS['pdf_columns'][$al][$spalte][$zn][$zsz]['typ']=1;
									$GLOBALS['pdf_columns'][$al][$spalte][$zn][$zsz]['xc']=$stxc;
									$GLOBALS['pdf_columns'][$al][$spalte][$zn][$zsz]['w']=$breite;
									$GLOBALS['pdf_columns'][$al][$spalte][$zn][$zsz]['h']=$hoehe;

									if((!$bar && (substr($bild, -10, 6) == 'bar_bg')) || ($bar > 0))
										$bar++;

									if($bar == 2)
									{
										$GLOBALS['pdf_columns'][$al][$spalte][$zn][$zsz]['typ']=2;
										$GLOBALS['pdf_columns'][$al][$spalte][$zn][$zsz]['xc']=0.00;
									}
									else
										$stxc += $breite;

									// restl. Text
									if($gre < strlen($pxval)-1)
									{
										$zsz++;
										if(strlen($leadspace))
											$npxval=$leadspace.trim(substr($pxval, $gre+1));
										else
											$npxval=$leadspace.substr($pxval, $gre+1);
//echo('<hr>['.$al.']['.$spalte.']['.$zn.']['.$zsz.'][txt]:<br />'.$npxval.'<br />');
										$sw=fxpdf_strlen($pdf, $npxval, $azeile['fs'], $azeile['md']);
										if($GLOBALS['pdf_get_col_width_debug'] == 1)
											echo('<div style="background:#ffffff;padding:2px;border-top:1px solid black;"><i>(L'.__LINE__.')</i> <b>$npxval</b> ['.fxHtmlEncode($npxval).'] -&gt; width='.$sw.'</div>');
										$GLOBALS['pdf_columns'][$al][$spalte][$zn][$zsz]=$azeile;
										$GLOBALS['pdf_columns'][$al][$spalte][$zn][$zsz]['txt']=$npxval;
										$GLOBALS['pdf_columns'][$al][$spalte][$zn][$zsz]['xc']=$stxc;
										$GLOBALS['pdf_columns'][$al][$spalte][$zn][$zsz]['w']=$sw;
										$stxc += $sw;
									}
								}
								$zsz++;
							}
						}
						else
						{
							$GLOBALS['pdf_columns'][$al][$spalte][$zn][$zsz]=$azeile;

							$GLOBALS['pdf_columns'][$al][$spalte][$zn][$zsz]['otxt']=$azeile['otxt'];
							$GLOBALS['pdf_columns'][$al][$spalte][$zn][$zsz]['txt']='';
							$GLOBALS['pdf_columns'][$al][$spalte][$zn][$zsz]['xc']=$stxc;
							$GLOBALS['pdf_columns'][$al][$spalte][$zn][$zsz]['w']=0.00;
						}

						// Führende Leerzeichen ermitteln
						if(($zn == 1) && !$GLOBALS['pdf_columns'][$al][$spalte][$zn][0]['typ'])
						{
							$calc_osl=strlen($GLOBALS['pdf_columns'][$al][$spalte][$zn][0]['txt']);
							$calc_tsl=strlen(ltrim($GLOBALS['pdf_columns'][$al][$spalte][$zn][0]['txt']));
							if($calc_tsl < $calc_osl)
								$leadspace=str_repeat(' ', $calc_osl-$calc_tsl);
						}

						// Prüfen, ob aktuelle Breite größer als bisherige Spaltenbreite ist
						$acw=$stxc + $GLOBALS['pdf_array']['xd']*2.0;
						if($endkom)
							$acw=0.00;
						$GLOBALS['pdf_columns'][$al][$spalte][$zn][$zsz]['cs']=$GLOBALS['print_array'][$al][$spalte]['cs'];
//fxDebug($GLOBALS['print_array'][$al][$spalte], $al.'-'.$spalte);
						if($acw > $GLOBALS['pdf_col_width'][$spalte])
						{
							// Falls keine Colspan-Spalte, dann größere Breite als Spaltenbreite verwenden
							if($GLOBALS['print_array'][$al][$spalte]['cs'] <= 1)
							{
								$GLOBALS['pdf_col_width'][$spalte]=$acw;
								if($GLOBALS['pdf_get_col_width_debug'] == 1)
									echo('<div style="background:#eeeeee;padding:2px;border-top:1px solid black;"><i>(L'.__LINE__.')</i> <b>Line '.$al.':</b> "'.fxHtmlEncode($zeile).'"<br />-&gt; Value ['.$spalte.']: width='.$GLOBALS['pdf_col_width'][$spalte].'</div>');
							}

							// Falls Breite auf mehrere Spalten verteilt ist, dann merken für eine evtl. spätere Anpassung
							if($GLOBALS['print_array'][$al][$spalte]['cs'] > 1)
							{
								$csarr[]=array(
									'al'		=> $al,
									'spalte'	=> $spalte,
									'acw'		=> $acw
								);
							}
						}
					}
				}
			}
		}
	}

	// Colspan-Spalten anpassen
	if(sizeof($csarr))
	{
//fxDebug($csarr, '$csarr');
		foreach($csarr as $cs)
		{
			$al	= $cs['al'];
			$spalte	= $cs['spalte'];
			$acw=$cs['acw'];

			$csc=$GLOBALS['print_array'][$al][$spalte]['cs'];
			// Gesamtbreite ermitteln
			$csw=$GLOBALS['pdf_col_width'][$spalte];
			$cc	=1;
			foreach($sc_array as $cspalte)
			{
				if($cspalte <= $spalte)
					continue;
				if($csc > 1)
					$csw += $GLOBALS['pdf_col_width'][$cspalte];
				else
					break;
				--$csc;
				$cc++;
			}
			// Spaltenbreite nur ändern, falls aktuelle Breite größer als Gesamtbreite
			if($csw < $acw)
			{
				$cdiff=($acw-$csw)/$cc;
				$csc=$GLOBALS['print_array'][$al][$spalte]['cs'];
				foreach($sc_array as $cspalte)
				{
					if($cspalte < $spalte)
						continue;
					if($cc > 0)
					{
						$GLOBALS['pdf_col_width'][$cspalte] += $cdiff;
						if($GLOBALS['pdf_get_col_width_debug'] == 1)
							echo('<i>(L'.__LINE__.')</i> Diff ['.$cspalte.']: width='.$GLOBALS['pdf_col_width'][$cspalte].'<br />');
					}
					else
						break;
					--$cc;
				}
			}
		}
	}
//fxDebug($GLOBALS['pdf_col_width'], 'pdf_col_width');
}

/**
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  DPDF_SUM                                                                 +
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf - Mandatory parameter: ???PARAMETER???
 * @param various $md   - Mandatory parameter: ???PARAMETER???
 */
function dpdf_sum(&$pdf, $md)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $summe, $print_array, $pdf_array, $pdf_col_width;

	// Übertrag bzw. Gesamtsumme
	if($pdf_array['h_sum'] && sizeof($summe))
	{
//fxDebug($summe, 'Übertrag bzw. Gesamtsumme -- Zeile '.__LINE__);
		$GLOBALS['xc1']=dpdf_get_start_x($GLOBALS['sts'], $GLOBALS['xmax']);
		$GLOBALS['yc1']=$GLOBALS['yc2'];
		$GLOBALS['yc2']=$GLOBALS['yc1'] + $pdf_array['h_sum'];

		$sc		= 1;
		$scol	= false;
		$sxc 	= false;
		$spalte=$GLOBALS['sts'];	// Startspalte
		$sexit	= false;			// Spaltenanzeige vorzeitig verlassen
		while(($spalte < $GLOBALS['mxs']) && !$sexit)
		{
			$rsp=$GLOBALS['rspalte'][$spalte];
			$GLOBALS['xc2']=$GLOBALS['xc1']+$pdf_col_width[$rsp];
			if($GLOBALS['xc2'] > $pdf_array['w_body'])
				$sexit=true;

			if(strlen($summe[$rsp]))
				$scol=true;
			if(isset($pdf_array['col_carry']))
				$bcol=strtoupper(substr(trim($pdf_array['col_carry']), -6));
			else
				$bcol='dddddd';
			if($md)
			{
				if(isset($pdf_array['col_total']))
					$bcol=strtoupper(substr(trim($pdf_array['col_total']), -6));
				else
					$bcol='d3e0e6';
			}
			if($scol)
			{
				$fb='1100';
				$txt='';
			}
			else
			{
				if($sc == 1)
					$fb='1100';
				else
					$fb='0100';
				if($md)
					$txt=meldung(1555, false);	// Gesamtsumme
				else
					$txt=meldung(1575, false);	// Übertrag
				if(!$sxc)
					$sxc=$GLOBALS['xc1']+$pdf_array['xd'];
				$txtlen=fxpdf_strlen($pdf, $txt, $pdf_array['fs'], 'B');
				if($sxc != 'no')
					$txtlen += $sxc;
			}
			fxpdf_box($pdf, $GLOBALS['xc1'], $GLOBALS['yc1'], $GLOBALS['xc2'], $GLOBALS['yc2'], $bcol, $fb, 0.0, 0.0, 0.35, '666666');
			if($txt && ($GLOBALS['xc1'] > $txtlen) && ($sxc != 'no'))
			{
				fxpdf_text($pdf, $txt, '!'.$sxc, $GLOBALS['yc1']+$pdf_array['fs']+$pdf_array['yd']*0.75, $pdf_array['fs'], 'left', 'bold');
				$sxc='no';
			}
			// Ausrichtung
			$align=strtolower($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$rsp]['horausr']);
			if($align == 'right')
				$xc=$GLOBALS['xc2']-$pdf_array['xd'];
			else if($align == 'center')
				$xc=($GLOBALS['xc1']+$GLOBALS['xc2'])/2;
			else
				$xc=$GLOBALS['xc1']+$pdf_array['xd'];
			if(strlen($summe[$rsp]))
			{
				$zwert=get_entry($summe[$rsp], $GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$rsp], false);
				$cur='';
				if($GLOBALS['fx_mfields'][$GLOBALS['_masknumber']][$rsp]['feldtyp'] == 11) // Waehrung
				{
					$cur=' '.$GLOBALS['waehrung'];
					$align='right';
					$xc=$GLOBALS['xc2']-$pdf_array['xd'];
				}
				fxpdf_text($pdf, $zwert.$cur, '!'.$xc, $GLOBALS['yc1']+$pdf_array['fs']+$pdf_array['yd']*0.75, $pdf_array['fs'], $align, 'bold');
			}
			$GLOBALS['xc1']=$GLOBALS['xc2'];
			$sc++;
			$spalte++;
		}
	}
}

/**
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  DPDF_CHEAD                                                               +
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf - Mandatory parameter: ???PARAMETER???
 * @param various $md   - Mandatory parameter: ???PARAMETER???
 * @param various $yc   - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dpdf_chead(&$pdf, $md, $yc)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $rechnung_array, $pdf_array;

	$yc1=$yc;
	$fac=$pdf_array['fac'];
	dpdf_set_fac(1.00);

	// Rechnungskopf
	if(fxIsArray($rechnung_array) && ($GLOBALS['config'] == 2))
	{
		$xc1=0;

		// Überschrift
		if($md)
		{
			// Rechnung
			$kopie='';
			if($_GET['rcp'])
				$kopie='  {COPY}';

			$rechnungstyp='';
			if(strlen($rechnung_array['kto']))
			{
				$chk_arr=explode(',', $rechnung_array['kto']);
				foreach($chk_arr as $c)
				{
					$rad_arr=explode('_', $c);
					if($rad_arr[0] == 2121)
					{
						$rechnungstyp=db_value("SELECT kommunikation_txt FROM kommunikation WHERE kommunikation_id=".(int)$rad_arr[1]." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
						break;
					}
				}
			}
			if(!strlen($rechnungstyp))
			{
				$rtp=(int)$rechnung_array['rechnungstyp'];
				if($rtp <= 0)
					$rechnungstyp=meldung(872, false);	// Rechnung
				else
					$rechnungstyp=db_value("SELECT kommunikation_txt FROM kommunikation WHERE kommunikation_id=".$rtp." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
			}

			fxpdf_text($pdf, $rechnungstyp.' ['.$rechnung_array['rechnn_f'].']'.$kopie, $xc1+$pdf_array['xd'], $yc1+$pdf_array['fs']+$pdf_array['yd'], $pdf_array['fsh'], 'left', 'bold');

			// Zeitspanne
			$rbd=trim($rechnung_array['ist_beg_dtm']);
			if(strlen($rbd))
				$rbd=get_entry($rbd, 'datum');
			$red=trim($rechnung_array['ist_end_dtm']);
			if(strlen($red))
				$red=get_entry($red, 'datum');
			if(strlen($rbd) || strlen($red))
			{
				if(!strlen($rbd))
					$rbd='*';
				if(!strlen($red))
					$red='*';
				fxpdf_text($pdf, '['.meldung(1105, true).': '.$rbd.'-'.$red.']', $xc1+$pdf_array['xd'], $yc1+$pdf_array['fs']+$pdf_array['yd']*2+$pdf_array['fsh'], $pdf_array['fs'], 'left');
			}
		}

		// Rechnungs-Info als Array aufbauen
		if(!sizeof($GLOBALS['ria']))
		{
			// Kundennummer
			if(strlen(trim($rechnung_array['kundennummer'])))
				$GLOBALS['ria'][get_text(31, '20').':']=trim($rechnung_array['kundennummer']);
			// USt.-ID Kunde
			if($rechnung_array['kunde'])
			{
				$sql="SELECT kommunikation_txt FROM kommunikation WHERE personen_id=".(int)$rechnung_array['kunde']." AND kommunikationstyp=1768 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$uik=db_value($sql);
				if($uik)
					$GLOBALS['ria'][meldung(1768, true).':']=trim($uik);
			}
			// Rechnungsnummer
			if(strlen(trim($rechnung_array['rechnn_f'])))
				$GLOBALS['ria'][get_text(1048, '20').':']=trim($rechnung_array['rechnn_f']);
			// Datum
			$GLOBALS['ria'][meldung(878, true).':']=get_entry($rechnung_array['datum'], 'datum');
			// USt.-ID
			if($pdf_array['v_ust'])
			{
				$dppos=strpos($pdf_array['v_ust'], ':');
				if($dppos !== false)
					$GLOBALS['ria'][substr($pdf_array['v_ust'], 0, $dppos+1)]=substr($pdf_array['v_ust'], $dppos+1);
				else
					$GLOBALS['ria'][meldung(1527, true).':']=$pdf_array['v_ust'];
			}
			// Steuernummer
			if($pdf_array['v_stn'])
			{
				$dppos=strpos($pdf_array['v_stn'], ':');
				if($dppos !== false)
					$GLOBALS['ria'][substr($pdf_array['v_stn'], 0, $dppos+1)]=substr($pdf_array['v_stn'], $dppos+1);
				else
					$GLOBALS['ria'][meldung(1564, true).':']=$pdf_array['v_stn'];
			}

			// Benutzererweiterung entspr. kto
			if(strlen($rechnung_array['kto']))
			{
				$chk_arr=explode(',', $rechnung_array['kto']);
				foreach($chk_arr as $c)
				{
					$rad_arr=explode('_', $c);
					if(($rad_arr[0] == 2113) || ($rad_arr[0] == 2121))	// Rechnungsanschrift + Rechnungstyp überpringen
						continue;

					if(isset($GLOBALS['_preview_invoice']))
					{
						$ktyp='(KTO '.$rad_arr[0].')';
						$ktxt='Test';
					}
					else
					{
						$ktyp=meldung((int)$rad_arr[0], false);
						$ktxt=db_value("SELECT kommunikation_txt FROM kommunikation WHERE kommunikation_id=".(int)$rad_arr[1]." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
					}
//fxDebug($ktxt, $ktyp);
					$GLOBALS['ria'][$ktyp.':']=$ktxt;
				}
			}
		}

		// Maximale Anzahl an Buchstaben ermitteln
		$fnts=$pdf_array['fs'];
		$maxl=0;
		$maxv=0;
		foreach($GLOBALS['ria'] as $literal => $value)
		{
			$llen=fxpdf_strlen($pdf, $literal, $fnts);
			if($llen > $maxl)
				$maxl=$llen;
			$vlen=fxpdf_strlen($pdf, $value, $fnts);
			if($vlen > $maxv)
				$maxv=$vlen;
		}
		$xc1=$pdf_array['w_body'] - $maxl - $maxv - fxpdf_strlen($pdf, '###', $fnts);
		foreach($GLOBALS['ria'] as $literal => $value)
		{
			if($md)
			{
				fxpdf_text($pdf, $literal, $xc1, $yc1+$fnts, $fnts, 'left', 'bold');
				fxpdf_text($pdf, $value, -$pdf_array['xd'], $yc1+$fnts, $fnts, 'right', 'normal');
			}
			$yc1 += $fnts;
		}

		// Vorgangs-Name
		if(strlen($rechnung_array['name_mutterprojekt']))
		{
			$yc1 += 6.0*$pdf_array['fac'];
			if($md)
				fxpdf_line($pdf, $pdf_array['xd'], $yc1, -$pdf_array['xd'], $yc1, 1.0, 1.0);
			$yc1 += 2.0*$pdf_array['fac'];
			$GLOBALS['rtv']=fxpdf_wraptext($pdf, $rechnung_array['name_mutterprojekt'], $pdf_array['fs'], 'B', $pdf_array['w_body']);
			foreach($GLOBALS['rtv'] as $lnr => $txt)
			{
				$yc1 += $pdf_array['fs'];
				if($md && ($yc1 < $pdf_array['h_body']))
				{
					fxpdf_text($pdf, $txt, $pdf_array['xd'], $yc1, $pdf_array['fs'], 'left', 'bold');
					unset($GLOBALS['rtv'][$lnr]);
				}
				else
					break;
			}
			$yc1 += 6.0*$pdf_array['fac'];
			if($md)
				fxpdf_line($pdf, $pdf_array['xd'], $yc1, -$pdf_array['xd'], $yc1, 1.0, 1.0);
			$yc1 += 2.0*$pdf_array['fac'];
		}

		// Allgemeines Textfeld
		if(strlen($rechnung_array['allg_textfeld']))
		{
			if(!strlen($rechnung_array['name_mutterprojekt']))
			{
				$yc1 += 6.0*$pdf_array['fac'];
				if($md)
					fxpdf_line($pdf, $pdf_array['xd'], $yc1, -$pdf_array['xd'], $yc1, 1.0, 1.0);
				$yc1 += 2.0*$pdf_array['fac'];
			}
			$GLOBALS['rta']=fxpdf_wraptext($pdf, $rechnung_array['allg_textfeld'], $pdf_array['fs'], '', $pdf_array['w_body']);
			foreach($GLOBALS['rta'] as $lnr => $txt)
			{
				$yc1 += $pdf_array['fs'];
				if($md && ($yc1 < $pdf_array['h_body']))
				{
					fxpdf_text($pdf, $txt, $pdf_array['xd'], $yc1, $pdf_array['fs']);
					unset($GLOBALS['rta'][$lnr]);
				}
				else
					break;
			}
		}

		// Y-Koordinate für nächten Abschnitt
		$yc1 += 16.0*$pdf_array['fac'];
	}

	dpdf_set_fac($fac);

	if($md)
		return $yc1;
	else
		return ($yc1 - $yc);
}

/**
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  DPDF_CFOOT                                                               +
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf - Mandatory parameter: ???PARAMETER???
 * @param various $md   - Mandatory parameter: ???PARAMETER???
 * @param various $yc   - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dpdf_cfoot(&$pdf, $md, $yc)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $rechnung_array, $rechnung_pos_array, $pdf_array;

	$yc1=$yc;
	$fac=$pdf_array['fac'];
	dpdf_set_fac(1.00);

	// Rechnungsfuss
	if(is_array($rechnung_array) && sizeof($rechnung_array) && ($GLOBALS['config'] == 2))
	{
		// Nettobetrag
		$cf[get_text(1072, '20')]=get_entry($rechnung_array['nettobetrag'], 'betrag', false).' '.$GLOBALS['waehrung'];
		// Mehrwertsteuer
		$mwst_txt=get_text(1069, '20');
		if(fxIsArray($rechnung_pos_array))
		{
			$mwa=array();
			foreach($rechnung_pos_array as $rp)
			{
				// Gültige Bündelungsposition oder Rechnungsposition?
				$r_pid=(int)$rp['projekt_id'];
				$r_bnd=(int)$rp['buendelpos'];
				$r_aid=(int)$rp['artikelzuord_id'];
				$valid=false;
				if(($r_pid == -1) && ($r_bnd == 0) && ($r_aid > 0))
					$valid=true;
				else if(($r_pid >= 0) && (($r_bnd < 1) || ($r_bnd > 98)))
					$valid=true;
				if($valid)
				{
					$mws=$rp['mwst_satz'];
					$mwv=$rp['mwst'];
					if(isset($mwa[$mws]))
						$mwa[$mws] += $mwv;
					else
						$mwa[$mws]=$mwv;
				}
			}
			if(sizeof($mwa))
			{
				foreach($mwa as $mws => $mwv)
					$cf[$mwst_txt.' '.get_entry($mws, 'dezimal').' %']=get_entry($mwv, 'betrag', false).' '.$GLOBALS['waehrung'];
			}
		}
		else
			$cf[$mwst_txt]=get_entry($rechnung_array['mehrwertsteuer'], 'betrag', false).' '.$GLOBALS['waehrung'];
		// Bruttobetrag
		$cf[get_text(1073, '20')]=get_entry($rechnung_array['bruttobetrag'], 'betrag', false).' '.$GLOBALS['waehrung'];

		// Maximale Anzahl an Buchstaben ermitteln
		$fnts=$pdf_array['fs'];
		$maxl=0;
		$maxv=0;
		foreach($cf as $literal => $value)
		{
			$llen=fxpdf_strlen($pdf, $literal, $fnts);
			if($llen > $maxl)
				$maxl=$llen;
			$vlen=fxpdf_strlen($pdf, $value, $fnts);
			if($vlen > $maxv)
				$maxv=$vlen;
		}
		$xc1=$pdf_array['w_body'] - $maxl - $maxv - fxpdf_strlen($pdf, '######', $fnts);

		$cfc=1;
		$cfs=sizeof($cf);
		$yc1 += $pdf_array['fs']+10.0*$pdf_array['fac'];
		foreach($cf as $literal => $value)
		{
			if($md)
			{
				fxpdf_text($pdf, $literal, $xc1, $yc1, $pdf_array['fs'], 'left', 'bold');
				fxpdf_text($pdf, $value, -$pdf_array['xd'], $yc1, $pdf_array['fs'], 'right', 'bold');
			}
			if($cfc < $cfs)
				$yc1 += $pdf_array['fs'];
			$cfc++;
			if($cfc == $cfs)
			{
				if($md)
				{
					// Schatten zeichen
					fxpdf_box($pdf, $pdf_array['w_body']+0.25*$pdf_array['fac'], $yc1-0.75*$pdf_array['fac'], $pdf_array['w_body']+2.25*$pdf_array['fac'], $yc1+$pdf_array['fs']+1.25*$pdf_array['fac'], 'eeeeee', '0000');
					fxpdf_box($pdf, $xc1+0.25*$pdf_array['fac'], $yc1+$pdf_array['fs']+1.25*$pdf_array['fac'], $pdf_array['w_body']+2.25*$pdf_array['fac'], $yc1+$pdf_array['fs']+3.25*$pdf_array['fac'], 'eeeeee', '0000');
					// Rechnungsbox
					if(isset($pdf_array['col_gross']))
						$bcol=strtoupper(substr(trim($pdf_array['col_gross']), -6));
					else
						$bcol='b5cfdc';
					fxpdf_box($pdf, $xc1-2.0*$pdf_array['fac'], $yc1-3.0*$pdf_array['fac'], $pdf_array['w_body'], $yc1+$pdf_array['fs']+1.0*$pdf_array['fac'], $bcol);
				}
				$yc1 += 6.0*$pdf_array['fac'];
			}
		}

		// Text
		$add_text_array=array();
		if(strlen($rechnung_array['allg_textfeld_fuss']))
		{
			$add_text_array[]='<:line:>';
			$add_text_array[]=$rechnung_array['allg_textfeld_fuss'];
		}
		if(strlen($pdf_array['v_text']))
		{
			if(sizeof($add_text_array))
				$add_text_array[]='<:space:>';
			else
				$add_text_array[]='<:line:>';
			$add_text_array[]=$pdf_array['v_text'];
		}
		if(strlen($rechnung_array['ktu']))
		{
			if(sizeof($add_text_array))
				$add_text_array[]='<:space:>';
			else
				$add_text_array[]='<:line:>';

			$chk_arr=explode(',', $rechnung_array['ktu']);
			foreach($chk_arr as $c)
			{
				$rad_arr=explode('_', $c);
				if(isset($GLOBALS['_preview_invoice']))
					$ktxt='(KTU '.$rad_arr[0].')';
				else
					$ktxt=db_value("SELECT kommunikation_txt FROM kommunikation WHERE kommunikation_id=".(int)$rad_arr[1]." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
				$add_text_array[]=$ktxt;
			}
		}
		if(sizeof($add_text_array))
		{
			foreach($add_text_array as $add_text)
			{
				if(($add_text == '<:line:>') || ($add_text == '<:space:>'))
				{
					$yc1 += 10.0*$pdf_array['fac'];
					if($md && ($add_text == '<:line:>'))
						fxpdf_line($pdf, $pdf_array['xd'], $yc1, -$pdf_array['xd'], $yc1, 1.0, 1.0);
				}
				else
				{
					$yc1 += 6.0*$pdf_array['fac'];
					$wt=fxpdf_wraptext($pdf, $add_text, $pdf_array['fs'], '', $pdf_array['w_body']);
					foreach($wt as $txt)
					{
						$yc1 += $pdf_array['fs'];
						if($md)
							fxpdf_text($pdf, $txt, $pdf_array['xd'], $yc1, $pdf_array['fs']);
					}
				}
			}
		}

		// Y-Koordinate für nächten Abschnitt
		$yc1 += 6.0*$pdf_array['fac'];
	}

	dpdf_set_fac($fac);

	if($md)
		return $yc1;
	else
		return ($yc1 - $yc);
}

/**
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  DPDFEXTRACTTEXT                                                          +
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various $line   - Mandatory parameter: ???PARAMETER???
 * @param various $column - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dpdfExtractText($line,$column)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//echo('<div style="border-radius:4px;border:1px solid #888888;padding:2px;margin-bottom:1px;width:1024px;background:#f0f0f0;"><b>dpdfExtractText</b> - $line='.$line.', $column='.$column.' <i class="blue">(LINE '.__LINE__.')</i><hr>');
	$eta=array();
	$lta=array('otxt'=>"", 'typ'=>0, 'txt'=>"", 'fs'=>$GLOBALS['pdf_array']['fs'], 'md'=>"normal", 'align'=>"", 'st'=>false, 'class'=>"", 'col'=>"", 'bcol'=>"", 'fcol'=>"");
	$txt=strtr($GLOBALS['print_array'][$line][$column]['show'], array(chr(13).chr(10)=>'', chr(10).chr(13)=>'', chr(13)=>'', chr(10)=>'', '&nbsp;'=>' '));
//echo('(LINE '.__LINE__.')<br />'.fxHtmlEncode($txt).'<hr />');
	$tl=0;
	if(!strlen($txt))
		$eta[$tl]=$lta;
	else
	{
		// Remove not displayable divs
		$sp=stripos($txt,'display:none');
		while($sp !== false)
		{
			$ep=$sp+12;
			$ts=strripos($txt,'<div',strlen($txt)-$sp);
			if($ts !== false)
			{
				$te=stripos($txt,'/div>',$sp);
				if($te !== false)
					$txt=substr($txt,0,$ts).substr($txt,$te+5);
				else
					$txt=substr($txt,0,$ts).substr($txt,$ep);
			}
			else
				$txt=substr($txt,0,$sp).substr($txt,$ep);
			$sp=stripos($txt,'tooltip=');
		}
		// Remove tooltips
		$sp=stripos($txt,'tooltip=');
		while($sp !== false)
		{
			$ep=$sp+8;
			while(($sp > 0) && (substr($txt,$sp-1,1) == ' '))
				$sp--;
			$q=substr($txt,$ep,1);
			$ep++;
			while($ep<strlen($txt))
			{
				if((substr($txt,$ep,1) == $q) && (substr($txt,$ep-1,1) != "\\"))
				{
					$ep++;
					break;
				}
				$ep++;
			}
			$txt=substr($txt,0,$sp).substr($txt,$ep);
			$sp=stripos($txt,'tooltip=');
		}
		// Remove last end tags, except arrows
		while((substr($txt,-1) == '>') && (substr($txt,-2) != '->'))
		{
			$sp=strlen($txt)-1;
			while(($sp >= 0) && (substr($txt,$sp,1) != '<'))
				$sp--;
			if(($sp < 0) || (substr($txt,$sp+1,1) != '/'))
				break;
			$txt=substr($txt,0,$sp);
		}
		// Remove multiple </div>
		$sp=stripos($txt,'</div></div>');
		while($sp !== false)
		{
			$txt=substr($txt,0,$sp).substr($txt,$sp+6);
			$sp=stripos($txt,'</div></div>');
		}
		// Remove ending </div> in table column
		$sp=stripos($txt,'</div></td>');
		while($sp !== false)
		{
			$txt=substr($txt,0,$sp).substr($txt,$sp+6);
			$sp=stripos($txt,'</div></td>');
		}
		// Insert and adjust breaks
		$txt=strtr($txt, array('</div>'=>'<br /><#>', '</tr>'=>'<br /><#>', '<BR>'=>'<br />'));
//echo('(LINE '.__LINE__.')<br />'.fxHtmlEncode($txt).'<hr />');

		// Extract text with parameters
		$ba=explode('<br />',$txt);
//fxDebug($ba,'$ba');
		foreach($ba as $bl => $t)
		{
			$ts=str_replace(' ','',$t);
//echo('<div style="background:#ffff00;color:#888888;padding:6px;border-top-left-radius:4px;border-top-right-radius:4px;">'.fxHtmlEncode($ts).'</div>');

			// Reuse previous attributes?
			$sp=stripos($t,'<div');
			if($sp === false)
				$sp=stripos($t,'<#>');
			if(!$bl || ($sp !== false))
				$eta[$tl]=$lta;
			else
			{
				$eta[$tl]=$eta[$tl-1];
				$eta[$tl]['txt']="";
				$eta[$tl]['special']="";
			}
			$eta[$tl]['otxt']=$t;

			// Find images <img...>
			$ima=array();
			$imc=0;
			while(true)
			{
				$sp=stripos($t,'<img');
				if($sp === false)
					break;
				$ep=stripos($t,'>',$sp+4);
				if($ep === false)
					$ep=strlen($t);

				$ima[$imc]=substr($t,$sp,$ep+1-$sp);
				$t=substr($t,0,$sp).'#IMG'.$imc.'#'.substr($t,$ep+1);
				$imc++;
			}
//fxDebug($ima,'$ima: $imc='.$imc);

			// Find horizontal lines <hr>
			$hra=array();
			$hrc=0;
			while(true)
			{
				$sp=stripos($t,'<hr');
				if($sp === false)
					break;
				$ep=stripos($t,'>',$sp+3);
				if($ep === false)
					$ep=strlen($t);

				$hra[$hrc]=substr($t,$sp,$ep+1-$sp);
				$t=substr($t,0,$sp).'#HR'.$hrc.'#'.substr($t,$ep+1);
				$hrc++;
			}
//fxDebug($hra,'$hra: $hrc='.$hrc);

			// Strip tags
			$it=false;
			$v='';
			for($p=0; $p<strlen($t); $p++)
			{
				$c=substr($t,$p,1);
				if(($c == '<') && !$it)
					$it=true;
				else if(($c == '>') && ($v != '-') && $it)
					$it=false;
				else if(!$it)
					$eta[$tl]['txt'] .= $c;
				$v=$c;
			}

			// Parse and insert images
			if(sizeof($ima))
			{
				foreach($ima as $imc => $imt)
				{
					$img_repl='';
					// Image name
					$img_src='';
					$sp=stripos($imt,'src=');
					if($sp !== false)
					{
						$sp += 4;
						$q=substr($imt,$sp,1);
						if(($q != '"') && ($q != "\'"))
							$q=' ';
						else
							$sp++;
						while($sp < strlen($imt))
						{
							$c=substr($imt,$sp,1);
							if($c == $q)
								break;
							$img_src .= $c;
							$sp++;
						}
					}
					if(strlen($img_src))
					{
//echo('$img_src='.$img_src.'<hr>');
						// Image width
						$img_width='';
						$sp=stripos($imt,'width=');
						if($sp === false)
							$sp=stripos($imt,'width:');
						if($sp !== false)
						{
							$sp += 6;
							while(($sp < strlen($imt)) && !is_numeric(substr($imt,$sp,1)))
								$sp++;
							while(($sp < strlen($imt)) && is_numeric(substr($imt,$sp,1)))
							{
								$img_width .= substr($imt,$sp,1);
								$sp++;
							}
						}
						$img_width=(int)$img_width;
//echo('$img_width='.$img_width.'<hr>');
						// Image height
						$img_height='';
						$sp=stripos($imt,'height=');
						if($sp === false)
							$sp=stripos($imt,'height:');
						if($sp !== false)
						{
							$sp += 7;
							while(($sp < strlen($imt)) && !is_numeric(substr($imt,$sp,1)))
								$sp++;
							while(($sp < strlen($imt)) && is_numeric(substr($imt,$sp,1)))
							{
								$img_height .= substr($imt,$sp,1);
								$sp++;
							}
						}
						$img_height=(int)$img_height;
//echo('$img_height='.$img_height.'<hr>');

						// Get real image width and height
						$size=fxpdf_picsize($img_src,1.0,$img_width,$img_height);
						if($size['width'])
							$img_width=$size['width'];
						if($size['height'])
							$img_height=$size['height'];

						$img_repl='{PX:';
						if($img_width)
							$img_repl .= '"w'.($img_width + 4.0*$GLOBALS['pdf_array']['fac']).':';
						if($img_height)
							$img_repl .= '"h'.($img_height + 2.0*$GLOBALS['pdf_array']['fac']).':';
						$img_repl .= '@'.$img_src.'}';
					}

					// Replace image
					$eta[$tl]['txt']=str_replace('#IMG'.$imc.'#', $img_repl, $eta[$tl]['txt']);
				}
			}

			// Parse and insert horizontal lines
			if(sizeof($hra))
			{
				foreach($hra as $hrc => $hrt)
				{
					// Color
					$hr_col='';
					$sp=stripos($hrt,'fxfhr');
					if($sp !== false)
						$hr_col='888888';
					else
					{
						$sp=stripos($hrt,'color=');
						if($sp !== false)
						{
							$sp += 6;
							$q=substr($hrt,$sp,1);
							if(($q != '"') && ($q != "\'"))
								$q=' ';
							else
								$sp++;
							while($sp < strlen($hrt))
							{
								$c=substr($hrt,$sp,1);
								if($c == $q)
									break;
								$hr_col .= $c;
								$sp++;
							}
						}
					}
					if(!strlen($hr_col))
						$hr_col='000000';
					else if(substr($hr_col,0,1) == '#')
						$hr_col=substr($hr_col.'000000',1,6);
					else
						$hr_col=substr($hr_col.'000000',0,6);

					$hr_repl='{HR:'.$hr_col.'}';

					// Replace horizonatl line
					$eta[$tl]['txt']=str_replace('#HR'.$hrc.'#', $hr_repl, $eta[$tl]['txt']);
				}
			}

			// Strip images
			while(true)
			{
				$isp=strpos($ts,'<img');
				if($isp === false)
					break;
				$iep=strpos($ts,'>',$isp+4);
				if($iep === false)
					$ts=substr($ts,0,$isp);
				else
					$ts=substr($ts,0,$isp).substr($ts,$iep+1);
			}
//echo('<div style="background:#ffffff;color:#000000;padding:6px;border-bottom-left-radius:4px;border-bottom-right-radius:4px;margin-bottom:4px;">'.fxHtmlEncode($ts).'</div>');

			// Mode
			// ...italic
			$sp=stripos($ts,'<i>');
			if($sp === false)
				$sp=stripos($ts,'</i>');
			if($sp === false)
				$sp=stripos($t,'<i ');
			if($sp === false)
				$sp=stripos($ts,'italic');
			if($sp !== false)
				$eta[$tl]['md']='italic';
			// ...bold
			$sp=stripos($ts,'<b>');
			if($sp === false)
				$sp=stripos($ts,'</b>');
			if($sp === false)
				$sp=stripos($t,'<b ');
			if($sp === false)
				$sp=stripos($ts,'bold');
			if($sp !== false)
				$eta[$tl]['md']='bold';

			// Strike
			if(stripos($ts,'<strike>') !== false)
				$eta[$tl]['st']=true;

			// Align
			// ...left
			$sp=stripos($ts,'"left"');
			if($sp === false)
				$sp=stripos($ts,':left');
			if($sp !== false)
				$eta[$tl]['align']='left';
			else
			{
				// ...right
				$sp=stripos($ts,'"right"');
				if($sp === false)
					$sp=stripos($ts,':right');
				if($sp !== false)
					$eta[$tl]['align']='right';
				else
				{
					// ...center
					$sp=stripos($ts,'"center"');
					if($sp === false)
						$sp=stripos($ts,':center');
					if($sp !== false)
						$eta[$tl]['align']='center';
				}
			}

			// Class
			$sp=stripos($t,'class=');
			if($sp !== false)
			{
				$sp += 6;
				$c=substr($t,$sp,1);
				while(($c == ' ') || ($c == '"') || ($c == '\''))
				{
					$sp++;
					$c=substr($t,$sp,1);
				}
				$clc=0;
				$cla=array();
				while($sp<strlen($t))
				{
					$c=substr($t,$sp,1);
					if(($c == '"') || ($c == '\'') || ($c == ';'))
						break;
					if($c == ' ')
						$clc++;
					else if(isset($cla[$clc]))
						$cla[$clc] .= $c;
					else
						$cla[$clc]=$c;
					$sp++;
				}
				if(sizeof($cla))
				{
					foreach($cla as $cls)
					{
						$cls=strtolower(trim($cls));
						if(strlen($cls))
						{
//echo('<div style="padding-left:24px;border-radius:4px;padding:2px;margin-bottom:1px;width:1024px;background:#e8e8e8;"><b>$v</b> - CLASS-ATTRIBUTE! <i class="blue">(LINE '.__LINE__.')</i><br />'.fxHtmlEncode($cls).'</div>');
							$eta[$tl]['class']=$cls;
							$bcls=$GLOBALS['bstyle'][$cls];
							if(strlen($bcls))
								$eta[$tl]['bcol']=$bcls;
							$ccls=$GLOBALS['cstyle'][$cls];
							if(strlen($ccls))
							{
								if(($cls == 'darkgrey') || ($cls == 'grey'))
									$eta[$tl]['col']='000000';
								else
									$eta[$tl]['col']=$ccls;

							}
							if(($cls == 'maskt') || (substr($cls,0,3) == 'rhd'))
								$eta[$tl]['md']='bold';
						}
					}
				}
			}

			// Color
			$sp=stripos($ts,'color:');
			if($sp === false)
				$sp=stripos($ts,'color=');
			if($sp !== false)
			{
				$sp += 6;
				$c=substr($ts,$sp,1);
				while(($c == '"') || ($c == '\''))
				{
					$sp++;
					$c=substr($ts,$sp,1);
				}
				$col='';
				while($sp<strlen($ts))
				{
					$c=substr($ts,$sp,1);
					if(($c == '"') || ($c == '\'') || ($c == ';'))
						break;
					$col .= $c;
					$sp++;
				}
				$col=strtolower(trim($col));
				if(strlen($col))
				{
					if(substr($col,0,1) == '#')
						$col=substr($col,1);
//echo('<div style="padding-left:24px;border-radius:4px;padding:2px;margin-bottom:1px;width:1024px;background:#e8e8e8;"><b>$v</b> - COLOR-ATTRIBUTE! <i class="blue">(LINE '.__LINE__.')</i><br />'.fxHtmlEncode($col).'</div>');
					$ccls=$GLOBALS['cstyle'][$col];
					if(strlen($ccls))
						$eta[$tl]['col']=$ccls;
					else if(strlen($col) == 6)
						$eta[$tl]['col']=$col;
				}
			}

			// Background-Color
			$sp=stripos($ts,'background:');
			if($sp !== false)
				$sp += 11;
			if($sp === false)
			{
				$sp=stripos($ts,'background-color:');
				if($sp !== false)
					$sp += 17;
			}
			if($sp === false)
			{
				$sp=stripos($ts,'bgcolor=');
				if($sp !== false)
					$sp += 8;
			}
			if($sp !== false)
			{
				$c=substr($ts,$sp,1);
				while(($c == '"') || ($c == '\''))
				{
					$sp++;
					$c=substr($ts,$sp,1);
				}
				$bcol='';
				while($sp<strlen($ts))
				{
					$c=substr($ts,$sp,1);
					if(($c == '"') || ($c == '\'') || ($c == ';'))
						break;
					$bcol .= $c;
					$sp++;
				}
				$bcol=strtolower(trim($bcol));
				if(strlen($bcol))
				{
					if(substr($bcol,0,1) == '#')
						$bcol=substr($bcol,1);
//echo('<div style="padding-left:24px;border-radius:4px;padding:2px;margin-bottom:1px;width:1024px;background:#e8e8e8;"><b>$v</b> - BACKGROUND-COLOR-ATTRIBUTE! <i class="blue">(LINE '.__LINE__.')</i><br />'.fxHtmlEncode($bcol).'</div>');
					$bcls=$GLOBALS['bstyle'][$bcol];
					if(strlen($bcls))
						$eta[$tl]['bcol']=$bcls;
					else if(strlen($bcol) == 6)
						$eta[$tl]['bcol']=$bcol;
				}
			}

			$tl++;
		}
	}
//fxDebug($eta,'$eta');
//echo('</div>');

	return $eta;
}

/**
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  DPDF_GET_START_X                                                         +
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various $spalte - Mandatory parameter: ???PARAMETER???
 * @param various $abzug  - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dpdf_get_start_x($spalte, $abzug)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$xc=0.00;
	for($sc=0; $sc<$spalte; $sc++)
		$xc += $GLOBALS['pdf_col_width'][$GLOBALS['rspalte'][$sc]];

	return $xc-$abzug;
}

/**
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  DPDF_SET_FAC                                                             +
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various $nfac - Mandatory parameter: ???PARAMETER???
 */
function dpdf_set_fac($nfac)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$GLOBALS['pdf_array']['fac']	= $nfac;
	$GLOBALS['pdf_array']['fs']		=  8.0*$nfac;	// Zeichensatzgrösse (Spalteneinträge)
	$GLOBALS['pdf_array']['fshl']	= 13.0*$nfac;	// Zeichensatzgrösse (Gesamtüberschrift)
	$GLOBALS['pdf_array']['fsh']	=  9.0*$nfac;	// Zeichensatzgrösse (Tabellenüberschrift)
	$GLOBALS['pdf_array']['xd']		=  3.0*$nfac;	// X-Distanz in Spalten (CELLSPACING)
	$GLOBALS['pdf_array']['yd']		=  3.0*$nfac;	// Y-Distanz in Spalten (CELLSPACING)
}
?>