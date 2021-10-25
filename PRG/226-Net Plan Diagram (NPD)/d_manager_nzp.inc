<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : d_manager_nzp.inc                                            //
// Version     : 21.2                                                         //
// Begin       : 2021-01-11                                                   //
// Last Change : 2020-01-11                                                   //
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
 * PF 226: Net Plan Diagram (NPD)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
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

$load_pdf_config_debug=0;
$pdf_function_debug=0;

if(isset($_preview_config))
	ob_start();
else
{
	setForm(true);
	fxSession('t','l');
	if(fxIsArray($_GET))
		extract($_GET);
}

// Can we create PDF...
$GLOBALS['_ext_pdf']='';
$GLOBALS['_ext_pdf_tcpdf']=true;
$req_fn=fxf_fn_reqFilename('ext_pdf');
if(strlen($req_fn))
	require($req_fn);

if(isset($GLOBALS['fxptdata']['pdf']) && is_array($GLOBALS['fxptdata']['pdf']) && sizeof($GLOBALS['fxptdata']['pdf']))
{
	// Rückumwandlungsarray der HTML-Sonderzeichen
	$trans_html=array_flip(get_html_translation_table(HTML_ENTITIES));
	$trans_html['&nbsp;']=chr(32);

	// Text
	$type_array=kategorie_array('projektart');				// Projektartbezeichnung
	$prio_array=fxf_getReferenceValues('prioritaet');		// Prioritätsbezeichnungen
	$stat_array=fxf_getReferenceValues('projekt_status');	// Projektstatusbezeichnungen
	$weekdays=fxf_getWeekdayNames(2);						// Kurze Wochentagsnamen: 0=So, 1=Mo, ..., 6=Sa


	$txt_append=meldung(1588, false);						// Anhang
	$txt_open='{'.meldung(1794, false).'}';					// offen
	$txt_nstart='{'.meldung(2833, false).'}';				// nocht nicht begonnen
	$txt_nzp=meldung(2631, false);							// Netzplandiagramm

	$txt_lit=explode('|', meldung(2831, false));			// Vorgangsart|Status|Priorität|Planungszeitraum (Soll)|Arbeitszeitraum (Ist)|Aktionsnachricht
	$leg_lit=explode('|', get_message('NP_Legend'));		// Nr.=Vorgangsnummer|Name=Vorgangsbezeichnung|D=Dauer (in Tagen)|GP=Gesamtpuffer (in Tagen)|FP=freier Puffer (in Tagen)|FB=frühester Beginn (in Tagen nach Projektstart)|FE=frühestes Ende (in Tagen nach Projektstart)|SB=spätester Beginn (in Tagen nach Projektstart)|SE=spätestes Ende (in Tagen nach Projektstart)

	$leg_arr=array(
		array('dk'=>"nr", 'dl'=>"", 'dt'=>""),
		array('dk'=>"name", 'dl'=>"", 'dt'=>""),
		array('dk'=>"len", 'dl'=>"", 'dt'=>""),
		array('dk'=>"gp", 'dl'=>"", 'dt'=>""),
		array('dk'=>"fp", 'dl'=>"", 'dt'=>""),
		array('dk'=>"fb", 'dl'=>"", 'dt'=>""),
		array('dk'=>"fe", 'dl'=>"", 'dt'=>""),
		array('dk'=>"sb", 'dl'=>"", 'dt'=>""),
		array('dk'=>"se", 'dl'=>"", 'dt'=>"")
	);
	foreach($leg_lit as $l => $lit)
	{
		$ep=strpos($lit, '=');
		$leg_arr[$l]['dl']=trim(substr($lit, 0, $ep));
		$leg_arr[$l]['dt']=trim(substr($lit, $ep));
	}

	// Hintergrundfarbe
	$cutcol='ffffff';

	// Skalierungsfaktor für Grafiken
	$pdf_picscale=0.65;

	// Einstellungswerte für Druckaufbereitung verwenden
	$settings_backup=$GLOBALS['fxpglobals']['settings'];
	$GLOBALS['fxpglobals']['settings']['formatdatum']=$GLOBALS['fxpglobals']['settings']['d_formatdatum'];
	$GLOBALS['fxpglobals']['settings']['formatzeit']=$GLOBALS['fxpglobals']['settings']['d_formatzeit'];
	$GLOBALS['fxpglobals']['settings']['deztrennzeichen']=$GLOBALS['fxpglobals']['settings']['d_deztrennzeich'];
	$GLOBALS['fxpglobals']['settings']['nkstellen']=$GLOBALS['fxpglobals']['settings']['d_nkstellen'];

	// Konfigurationsfile einladen und parsen: -3=cfg_pnzp.cfg (Netzplandiagramm)
	$pdf_array=load_pdfconfig(-3);
	// ...Zusatzinfos zum Array
	$pdf_array['filename']='fx_nzp_pdf';																		// Filename der zu erzeugenden PDF-Datei
	$pdf_array['h_foot']=8.0;																					// Höhe der Fusszeile (= Abstand zur Ferox-Fusszeile)
	$pdf_array['h_pfoot']=16.0;																					// Höhe der FeRox-Fusszeile
	$pdf_array['fheight']=7.0;																					// Fontgröße
	$pdf_array['ffheight']=$pdf_array['fheight'];																// Skalierte Fontgröße
	$pdf_array['min_y']=$pdf_array['tmargin'];																	// Obere Begrenzung
	$pdf_array['max_y']=$pdf_array['bmargin'];																	// Untere Begrenzung
	$pdf_array['w_body']=$pdf_array['width']-$pdf_array['lmargin']-$pdf_array['rmargin'];						// Innere Breite
	$pdf_array['h_body']=$pdf_array['height']-$pdf_array['tmargin']-$pdf_array['bmargin']-$pdf_array['hfoot'];	// Innere Höhe
	if(isset($pdf_array['f_nofoot']))
	{
		$pdf_array['h_foot']=0.0;
		$pdf_array['h_pfoot']=0.0;
	}
	else
		$pdf_array['h_body'] -=	$pdf_array['h_pfoot'];
//fxDebug($pdf_array, '$pdf_array', 0);

	// NZP-Variablen
	$top_vn='';

	while(true)
	{
		$pdf_array['ffheight']=$pdf_array['fheight']*$GLOBALS['pdf_array']['fac'];

		$ne_width=136.0*$pdf_array['fac'];	// 190
		$ne_height=58.0*$pdf_array['fac'];	// 90
		$ne_tgap=14.0*$pdf_array['fac'];	// 18
		$ne_gap=$ne_tgap*4;
		$ne_tpad=round($ne_height/4, 2) - 5.0*$pdf_array['fac'];
		$ne_cwidth=round($ne_width/3, 2);
		$ne_lheight=round($ne_height/2, 2);
		$ne_ltop2=$ne_lheight;
		$nb_width=$ne_width + 2*$ne_tgap;
		$nb_height=$ne_height + 2*$ne_tgap;

		$ne_twidth=$ne_tgap*2;
		$ne_theight=$ne_tgap - 2.0*$pdf_array['fac'];

		$nwidth=2.0*$ne_tgap + $GLOBALS['fxptdata']['pdf']['prj_info_arr']['var']['columns']*$ne_width + ($GLOBALS['fxptdata']['pdf']['prj_info_arr']['var']['columns']-1)*$ne_gap + 2.0*$pdf_array['fac'];
		$nheight=2.0*$ne_tgap + $GLOBALS['fxptdata']['pdf']['prj_info_arr']['var']['lines']*$ne_height + ($GLOBALS['fxptdata']['pdf']['prj_info_arr']['var']['lines']-1)*$ne_gap + 4.0*$pdf_array['fac'];
//echo('$nwidth='.$nwidth.', $nheight='.$nheight.'<br />');

		if($pdf_array['fit'] && ($nwidth > $pdf_array['w_body']))
		{
			$nf=floor($pdf_array['w_body'])/ceil($nwidth);
			$pdf_array['fac']=$nf;
		}
		else
			break;
	}

	$xg=$pdf_array['fac']*3.0;
	$xd=$pdf_array['ffheight']/6;
	$yd=$pdf_array['ffheight']/3;

	// Anzahl Seiten und Subseiten ermitteln
	$pyoffset=50.0;

	$pages=1;
	$pheight=$pdf_array['h_body'];
	$theight=$pyoffset+$nheight;
	while($pheight < $theight)
	{
		$pages++;
		$pheight += $pdf_array['h_body'];
	}
	$subpages=0;
	$pwidth=$pdf_array['w_body'];
	while($pwidth < $nwidth)
	{
		$subpages++;
		$pwidth += $pdf_array['w_body'];
	}
	$pxoffset=round(($pwidth-$nwidth)/2, 2);
//echo('$pages='.$pages.', $subpages=0-'.$subpages.' (~ '.($subpages+1).') -- $pheight='.$pheight.', $pwidth='.$pwidth.', -- $pyoffset='.$pyoffset.', $pxoffset='.$pxoffset.'<br />'.$nl);

	// Koordinaten festlegen
	foreach($GLOBALS['fxptdata']['pdf']['prj_info_arr'] as $vn => $data)
	{
		if($vn != 'var')
		{
			if(!strlen($top_vn))
				$top_vn=$vn;

			$etop=$data['line']*$nb_height + $data['line']*2*$ne_tgap;
			$eleft=$data['column']*$nb_width + $data['column']*2*$ne_tgap;

			$GLOBALS['fxptdata']['pdf']['prj_info_arr'][$vn]['top']=$pyoffset+$etop;
			$GLOBALS['fxptdata']['pdf']['prj_info_arr'][$vn]['left']=$pxoffset+$eleft;
//echo($vn.': etop='.$etop.', eleft='.$eleft.'<br />');
		}
	}

	// Ein neues PDF-Objekt erstellen mit Standard-Fehlerhandling und -Speicherverwaltung.
	$pdf=fxpdf_new();
	if($pdf)
	{
		// PDF-Dokument öffnen und Infofelder setzen
		fxpdf_page($pdf, 'open');
		// Fusszeileninfo ermitteln
		$dmb=fxpdf_foot_info();

		for($page=1; $page<=$pages; $page++)
		{
			for($subpage=0; $subpage<=$subpages; $subpage++)
			{
				// Neue Seite im vordefinierten Format erstellen.
				fxpdf_page($pdf, 'begin', array($pdf_array['width'], $pdf_array['height']));

				// Body
				fxpdf_clipping($pdf, 0, false, true);

				$ymin=($page-1)*$pdf_array['h_body'];
				$ymax=$page*$pdf_array['h_body'];
				$xmin=$subpage*$pdf_array['w_body'];
				$xmax=($subpage+1)*$pdf_array['w_body'];
//echo('$page='.$page.', $subpage='.$subpage.' -- $ymin='.$ymin.', $ymax='.$ymax.', $xmin='.$xmin.', $xmax='.$xmax.'<br />'.$nl);

				// Headline
				if($page == 1)
				{
					// ...Netzplandiagramm
					fxpdf_font($pdf, -16.0);
					$lpt=$pdf->GetStringWidth($txt_nzp);
					if(8.0+$lpt > $pwidth-16.0)
					{
						$tw=fxpdf_wraptext($pdf, $txt_nzp, -16.0, 'B', $pwidth-28.0);
						$txt_nzp=trim($tw[0]).'...';
						$lpt=$pdf->GetStringWidth($txt_nzp);
					}
					fxpdf_text($pdf, $txt_nzp, '!'.(8.0-$xmin), 20.0, -16.0, 'left', 'bold', -0.25, '000000');
//echo('<b>fxpdf_text</b>(!'.(8.0-$xmin).', 20.0, ...)<br />');

					// ...Vorgangsnummer + -name
					$xv=$lpt+40.0;
					if($xv < $pwidth-40.0)
					{
						$txt=$GLOBALS['fxptdata']['pdf']['prj_info_arr'][$top_vn]['nr'].': '.strtr($GLOBALS['fxptdata']['pdf']['prj_info_arr'][$top_vn]['name'], $trans_html);
						fxpdf_font($pdf, -12.0);
						$lpt=$pdf->GetStringWidth($txt);
						if($xv+$lpt > $pwidth-12.0)
						{
							$tw=fxpdf_wraptext($pdf, $txt, -12.0, 'B', $pwidth-$xv-20.0);
							$txt=trim($tw[0]).'...';
						}
						fxpdf_text($pdf, $txt, '!'.($xv-$xmin), 20.0, -12.0, 'left', 'bold', -0.25, '666666');
					}

					fxpdf_line($pdf, '!'.(1.0-$xmin), 30.0, '!'.($pwidth-4.0-$xmin), 30.0, 0.0, 0.0, -0.25, '666666');
				}

				// Alle Elemente durchgehen
				foreach($GLOBALS['fxptdata']['pdf']['prj_info_arr'] as $vn => $data)
				{
					if($vn != 'var')
					{
						$y1=$data['top']-$ymin;
						$y2=$y1+$nb_height;
						$x1=$data['left']-$xmin;
						$x2=$x1+$nb_width;
						// Element im Anzeigebereich?
						// ...JA, Element ist im Anzeigebereich
						if(fxpdf_is_visible($x1, $y1, $x2, $y2))
						{
//echo('<font color="#008800">'.$vn.'</font>: $x1='.$x1.', $y1='.$y1.' - $x2='.$x2.', $y2='.$y2.'<br />'.$nl);
//								fxpdf_rec($pdf, '!'.$x1, '!'.$y1, '!'.$x2, '!'.$y2, 1.0, 1.0, 0.10, 'dddddd');

							// Element
							// ...Background
							if(strlen($data['disp']['image']))
								fxpdf_gbox($pdf, '!'.($x1+$ne_tgap), '!'.($y1+$ne_tgap), '!'.($x2-$ne_tgap), '!'.($y2-$ne_tgap), 'ffffff', 'dddddd', array(0,1,0,0));
							else
								fxpdf_box($pdf, '!'.($x1+$ne_tgap), '!'.($y1+$ne_tgap), '!'.($x2-$ne_tgap), '!'.($y2-$ne_tgap), 'ffffff', '0000');
							// ...Lines
							fxpdf_line($pdf, '!'.($x1+$ne_tgap+$pdf_array['fac']), '!'.($y1+$ne_tgap+$ne_lheight), '!'.($x2-$ne_tgap-$pdf_array['fac']), '!'.($y1+$ne_tgap+$ne_lheight), 0.0, 0.0, 0.35, 'aaaaaa');
							fxpdf_line($pdf, '!'.($x1+$ne_tgap+$ne_cwidth), '!'.($y1+$ne_tgap+$pdf_array['fac']), '!'.($x1+$ne_tgap+$ne_cwidth), '!'.($y2-$ne_tgap-$pdf_array['fac']), 0.0, 0.0, 0.35, 'aaaaaa');
							fxpdf_line($pdf, '!'.($x2-$ne_tgap-$ne_cwidth), '!'.($y1+$ne_tgap+$ne_lheight), '!'.($x2-$ne_tgap-$ne_cwidth), '!'.($y2-$ne_tgap-$pdf_array['fac']), 0.0, 0.0, 0.35, 'aaaaaa');
							// ...Border
							$bd=0.0;
							if($data['disp']['border-style'] == "dotted")
							{
								$bd=1.0;
								if(strlen($GLOBALS['fxptdata']['pdf']['prj_info_arr']['var']['cp_vn']) && $data['cp'])
									$bd=2.0;
							}
							fxpdf_rec($pdf, '!'.($x1+$ne_tgap), '!'.($y1+$ne_tgap), '!'.($x2-$ne_tgap), '!'.($y2-$ne_tgap), $bd, $bd, 0.50+(0.25*($data['disp']['border-width']-1)), $data['disp']['border-color']);

							// Nr.
							fxpdf_text($pdf, $data['nr'], '!'.($x1+$ne_tgap+$ne_cwidth/2-$xd), '!'.($y1+$ne_tgap+$ne_lheight/2+$yd), $pdf_array['fheight'], 'center', 'bold', 0.35, '111111');

							// Name
							$txt=fxpdf_wraptext($pdf, strtr($data['name'], $trans_html), $pdf_array['fheight'], '', $ne_cwidth*1.80);
							$s=1;
							$l=$y1+$ne_tgap+0.50*$pdf_array['ffheight'];
							$x=$x2-$ne_tgap-$ne_cwidth-$xd;
							$y=$y1+$ne_tgap+$ne_lheight/2+$yd;
							while($s < sizeof($txt))
							{
								$ny=$y-($pdf_array['ffheight']/2);
								if($ny > $l)
								{
									$y=$ny;
									$s++;
								}
								else
									break;
							}
							for($c=0; $c<$s; $c++)
							{
								fxpdf_text($pdf, $txt[$c], '!'.$x, '!'.$y, $pdf_array['fheight'], 'center', 'normal', 0.25, '111111');
								$y += $pdf_array['ffheight'];
							}

							// D
							fxpdf_text($pdf, $data['len'], '!'.($x1+$ne_tgap+$ne_cwidth/2-$xd), '!'.($y2-$ne_tgap-$ne_lheight/2+$yd), $pdf_array['fheight'], 'center', 'normal', 0.25, '111111');

							// GP
							fxpdf_text($pdf, $data['gp'], '!'.($x1+$ne_tgap+$ne_cwidth*3/2-$xd), '!'.($y2-$ne_tgap-$ne_lheight/2+$yd), $pdf_array['fheight'], 'center', 'normal', 0.25, '111111');

							// FP
							fxpdf_text($pdf, $data['fp'], '!'.($x2-$ne_tgap-$ne_cwidth/2-$xd), '!'.($y2-$ne_tgap-$ne_lheight/2+$yd), $pdf_array['fheight'], 'center', 'normal', 0.25, '111111');

							// Datum
							$fbd=get_entry($data['fbd'], "datum");	// #333333
							$fed=get_entry($data['fed'], "datum");	// #333333
							$sbd=get_entry($data['sbd'], "datum");	// #999999
							$sed=get_entry($data['sed'], "datum");	// #999999

							// FB
							fxpdf_rec($pdf, '!'.$x1, '!'.$y1, '!'.($x1+$ne_twidth), '!'.($y1+$ne_theight), 0.0, 0.0, 0.35, 'aaccaa');
							fxpdf_text($pdf, $data['fb'], '!'.($x1+$ne_twidth/2-$xd), '!'.($y1+$ne_theight/2+$yd), $pdf_array['fheight'], 'center', 'bold', 0.30, '111111');
							fxpdf_text($pdf, $fbd, '!'.($x1+$ne_twidth+$xg), '!'.($y1+$ne_theight/2+$yd), $pdf_array['fheight']*0.75, 'left', 'normal', 0.20, '333333');

							// FE
							fxpdf_rec($pdf, '!'.($x2-$ne_twidth), '!'.$y1, '!'.$x2, '!'.($y1+$ne_theight), 0.0, 0.0, 0.35, 'aaccaa');
							fxpdf_text($pdf, $data['fe'], '!'.($x2-$ne_twidth/2-$xd), '!'.($y1+$ne_theight/2+$yd), $pdf_array['fheight'], 'center', 'bold', 0.30, '111111');
							fxpdf_text($pdf, $fed, '!'.($x2-$ne_twidth-$xg), '!'.($y1+$ne_theight/2+$yd), $pdf_array['fheight']*0.75, 'right', 'normal', 0.20, '333333');

							// SB
							fxpdf_rec($pdf, '!'.$x1, '!'.($y2-$ne_theight), '!'.($x1+$ne_twidth), '!'.$y2, 0.0, 0.0, 0.35, 'ccaaaa');
							fxpdf_text($pdf, $data['sb'], '!'.($x1+$ne_twidth/2-$xd), '!'.($y2-$ne_theight/2+$yd), $pdf_array['fheight'], 'center', 'normal', 0.25, '333333');
							fxpdf_text($pdf, $sbd, '!'.($x1+$ne_twidth+$xg), '!'.($y2-$ne_theight/2+$yd), $pdf_array['fheight']*0.75, 'left', 'normal', 0.10, '999999');

							// SE
							fxpdf_rec($pdf, '!'.($x2-$ne_twidth), '!'.($y2-$ne_theight), '!'.$x2, '!'.$y2, 0.0, 0.0, 0.35, 'ccaaaa');
							fxpdf_text($pdf, $data['se'], '!'.($x2-$ne_twidth/2-$xd), '!'.($y2-$ne_theight/2+$yd), $pdf_array['fheight'], 'center', 'normal', 0.25, '333333');
							fxpdf_text($pdf, $sed, '!'.($x2-$ne_twidth-$xg), '!'.($y2-$ne_theight/2+$yd), $pdf_array['fheight']*0.75, 'right', 'normal', 0.10, '999999');
						}
						// ...NEIN, Element ist nicht im Anzeigebereich
						else
						{
//echo('<font color="#880000">'.$vn.'</font>: $x1='.$x1.', $y1='.$y1.' - $x2='.$x2.', $y2='.$y2.'<br />'.$nl);
						}
					}
				}

				// Linien zum vorherigen Vorgang
				foreach($GLOBALS['fxptdata']['pdf']['prj_info_arr'] as $vn => $data)
				{
					if(($vn != 'var') && (!strlen($GLOBALS['fxptdata']['pdf']['prj_info_arr']['var']['cp_vn']) || !$data['cp']))
						fxpdf_nzpLine($pdf, $vn, '333333', 0.50);
				}
				// Linie des kritischen Pfades zum vorherigen Vorgang
				foreach($GLOBALS['fxptdata']['pdf']['prj_info_arr'] as $vn => $data)
				{
					if(($vn != 'var') && strlen($GLOBALS['fxptdata']['pdf']['prj_info_arr']['var']['cp_vn']) && $data['cp'])
						fxpdf_nzpLine($pdf, $vn, 'cc4444', 0.75);
				}

				// Alles abschneiden, was über den Rändern hinausgezeichnet wurde
				// ...Oben
				if($page > 1)
					fxpdf_box($pdf, '&0', '&0', '&-0', '!0', $cutcol, '0000');
				// ...Unten
				if($page < $pages)
					fxpdf_box($pdf, '&0', '!'.$pdf_array['h_body'], '&-0', '&-0', $cutcol, '0000');
				// ...Links
				if($subpage)
					fxpdf_box($pdf, '&0', '&0', '!0', '&-0', $cutcol, '0000');
				// ...Rechts
				if($subpage < $subpages)
					fxpdf_box($pdf, '!'.$pdf_array['w_body'], '&0', '&-0', '&-0', $cutcol, '0000');

				// Schneideränder zeichnen
				// ...Oben
				if($page > 1)
					fxpdf_line($pdf, '&0', '!0', '&-0', '!0', 2.0, 1.0, -0.15, '444444');
				// ...Unten
				if($page < $pages)
					fxpdf_line($pdf, '&0', '!'.$pdf_array['h_body'], '&-0', '!'.$pdf_array['h_body'], 2.0, 1.0, -0.15, '444444');
				// ...Links
				if($subpage)
					fxpdf_line($pdf, '!0', '&0', '!0', '&-0', 2.0, 1.0, -0.15, '444444');
				// ...Rechts
				if($subpage < $subpages)
					fxpdf_line($pdf, '!'.$pdf_array['w_body'], '&0', '!'.$pdf_array['w_body'], '&-0', 2.0, 1.0, -0.15, '444444');

				// Testversion
				if($testversion)
					fxpdf_diagonal($pdf, true);

				// FeRox-Fusszeile setzen, falls nicht abgeschaltet
				if(!isset($pdf_array['f_nofoot']))
				{
					$fptext=$page;
					if($subpages)
						$fptext .= '.'.fxpdf_char($subpage);
					if(($pages > 1) || $subpages)
					{
						$fptext .= ' / '.$pages;
						if($subpages)
							$fptext .= '.'.fxpdf_char($subpages);
					}
					fxpdf_clipping($pdf, 2, $fptext, true, $dmb);
				}
			}
		}

		// Anhang
		if($pdf_array['att'])
		{
			$pdf_array_backup=$pdf_array;

			$pdf_array['width']		=	595.353535352;
			$pdf_array['height']	=	841.999999997;
			$pdf_array['lmargin']	=	56.7003367002;
			$pdf_array['rmargin']	=	28.3501683501;
			$pdf_array['tmargin']	=	28.3501683501;
			$pdf_array['bmargin']	=	28.3501683501;
			$pdf_array['fac']		=	1.0;
			$pdf_array['lsfmt']		=	0;
			$pdf_array['fheight']	=	6.0;
			$pdf_array['ffheight']	=	$pdf_array['fheight']*$pdf_array['fac'];

			// Neue Seite im DIN-A4 Hochformat erstellen.
			fxpdf_page($pdf, 'begin', array($pdf_array['width'], $pdf_array['height']));
			$pdf->SetMargins($pdf_array['lmargin'], $pdf_array['tmargin'], $pdf_array['rmargin']);

			// Body
			fxpdf_clipping($pdf, 0, false, true);
//fxDebug($GLOBALS['pdf_array'], 'pdf_array (NEW)');

			// Überschrift: Anhang
			fxpdf_text($pdf, $txt_append, 8.0, 20.0, -16.0, 'left', 'bold', -0.25, '000000');
			fxpdf_line($pdf, 1.0, 30.0, -4.0, 30.0, 0.0, 0.0, 0.25, '666666');

			// Alle Tooltips durchgehen
			$ttda	= array();
			$apage	= 0;
			$y		= 50.0;
			$x1		= 1.0;
			$x2		= $x1+70.0;
			$x3		= $pdf_array['w_body']/2;
			$x4		= $x3+160.0;
			while(true)
			{
				$finished=true;
				foreach($GLOBALS['fxptdata']['pdf']['prj_info_arr'] as $vn => $data)
				{
					if($finished && ($vn != 'var') && !isset($ttda[$vn]))
					{
						$ys=$y;

						// ...Vorgangsnummer + -name
						fxpdf_gbox($pdf, $x1, $y, -1.0, $y+2.0*$pdf_array['ffheight'], 'ffffff', 'dddddd', array(0,1,0,0));
						$vt=fxpdf_wraptext($pdf, $data['nr'].': '.strtr($data['name'], $trans_html), $pdf_array['fheight']*1.25, 'B', $pdf_array['w_body']-30.0);
						$txt=$vt[0];
						if(sizeof($vt) > 1)
							$txt .= '...';
						fxpdf_text($pdf, $txt, $x1+8.0, $y+$pdf_array['ffheight']*1.50, $pdf_array['fheight']*1.25, 'left', 'bold', 0.25, '000066');
						$y += 3.50*$pdf_array['ffheight'];

						// ...Vorgangsart
						fxpdf_text($pdf, $txt_lit[0].':', $x1+2.0, $y, $pdf_array['fheight'], 'left', 'bold', 0.20, '000000');
						fxpdf_text($pdf, $type_array[$data['ptype']], $x2+4.0, $y, $pdf_array['fheight'], 'left', 'normal', 0.01, '000000');
						// ...D=Dauer (in Tagen)
						fxpdf_text($pdf, $leg_arr[2]['dl'], $x3, $y, $pdf_array['fheight'], 'left', 'bold', 0.20, '000000');
						fxpdf_text($pdf, $leg_arr[2]['dt'], $x3+15.0, $y, $pdf_array['fheight'], 'left', 'normal', 0.10, '000000');
						fxpdf_text($pdf, $data['len'], $x4, $y, $pdf_array['fheight'], 'left', 'normal', 0.01, '000000');
						$y += 1.50*$pdf_array['ffheight'];

						// ...Status
						fxpdf_text($pdf, $txt_lit[1].':', $x1+2.0, $y, $pdf_array['fheight'], 'left', 'bold', 0.20, '000000');
						fxpdf_text($pdf, $stat_array[$data['stat']], $x2+4.0, $y, $pdf_array['fheight'], 'left', 'normal', 0.01, '000000');
						// ...GP=Gesamtpuffer (in Tagen)
						fxpdf_text($pdf, $leg_arr[3]['dl'], $x3, $y, $pdf_array['fheight'], 'left', 'bold', 0.20, '000000');
						fxpdf_text($pdf, $leg_arr[3]['dt'], $x3+15.0, $y, $pdf_array['fheight'], 'left', 'normal', 0.10, '000000');
						fxpdf_text($pdf, $data['gp'], $x4, $y, $pdf_array['fheight'], 'left', 'normal', 0.01, '000000');
						$y += 1.50*$pdf_array['ffheight'];

						// ...Priorität
						fxpdf_text($pdf, $txt_lit[2].':', $x1+2.0, $y, $pdf_array['fheight'], 'left', 'bold', 0.20, '000000');
						fxpdf_text($pdf, $prio_array[$data['prio']], $x2+4.0, $y, $pdf_array['fheight'], 'left', 'normal', 0.01, '000000');
						// ...FP=freier Puffer (in Tagen)
						fxpdf_text($pdf, $leg_arr[4]['dl'], $x3, $y, $pdf_array['fheight'], 'left', 'bold', 0.20, '000000');
						fxpdf_text($pdf, $leg_arr[4]['dt'], $x3+15.0, $y, $pdf_array['fheight'], 'left', 'normal', 0.10, '000000');
						fxpdf_text($pdf, $data['fp'], $x4, $y, $pdf_array['fheight'], 'left', 'normal', 0.01, '000000');
						$y += 1.50*$pdf_array['ffheight'];

						// ...Planungszeitraum (Soll)
//							fxpdf_text($pdf, $txt_lit[3].':', $x1+2.0, $y, $pdf_array['fheight'], 'left', 'bold', 0.20, '000000');
//							fxpdf_text($pdf, get_entry($data['ps'], "datum").' - '.get_entry($data['pe'], "datum"), $x2+4.0, $y, $pdf_array['fheight'], 'left', 'normal', 0.01, '000000');

						// ...Arbeitszeitraum (Ist)
						if(strlen($data['as']))
						{
							$azi=get_entry($data['as'], "datum").' - ';
							if(strlen($data['ae']))
								$azi .= get_entry($data['ae'], "datum");
							else
								$azi .= $txt_open;	// offen
						}
						else
							$azi=$txt_nstart;	// noch nicht begonnen
						$ptt .= '</td></tr>';
						fxpdf_text($pdf, $txt_lit[4].':', $x1+2.0, $y, $pdf_array['fheight'], 'left', 'bold', 0.20, '000000');
						fxpdf_text($pdf, $azi, $x2+4.0, $y, $pdf_array['fheight'], 'left', 'normal', 0.01, '000000');
						// ...FB=frühester Beginn (in Tagen nach Projektstart)
						fxpdf_text($pdf, $leg_arr[5]['dl'], $x3, $y, $pdf_array['fheight'], 'left', 'bold', 0.20, '000000');
						fxpdf_text($pdf, $leg_arr[5]['dt'], $x3+15.0, $y, $pdf_array['fheight'], 'left', 'normal', 0.10, '000000');
						fxpdf_text($pdf, $data['fb'], $x4, $y, $pdf_array['fheight'], 'left', 'normal', 0.01, '000000');
						fxpdf_text($pdf, '(= '.$weekdays[getWeekday($data['fbd'])].', '.get_entry($data['fbd'], "datum").')', -3.0, $y, $pdf_array['fheight'], 'right', 'normal', 0.01, '000000');
						$y += 1.50*$pdf_array['ffheight'];

						// ...FE=frühestes Ende (in Tagen nach Projektstart)
						fxpdf_text($pdf, $leg_arr[6]['dl'], $x3, $y, $pdf_array['fheight'], 'left', 'bold', 0.20, '000000');
						fxpdf_text($pdf, $leg_arr[6]['dt'], $x3+15.0, $y, $pdf_array['fheight'], 'left', 'normal', 0.10, '000000');
						fxpdf_text($pdf, $data['fe'], $x4, $y, $pdf_array['fheight'], 'left', 'normal', 0.01, '000000');
						fxpdf_text($pdf, '(= '.$weekdays[getWeekday($data['fed'])].', '.get_entry($data['fed'], "datum").')', -3.0, $y, $pdf_array['fheight'], 'right', 'normal', 0.01, '000000');
						$y += 1.50*$pdf_array['ffheight'];

						// ...SB=spätester Beginn (in Tagen nach Projektstart)
						fxpdf_text($pdf, $leg_arr[7]['dl'], $x3, $y, $pdf_array['fheight'], 'left', 'bold', 0.20, '000000');
						fxpdf_text($pdf, $leg_arr[7]['dt'], $x3+15.0, $y, $pdf_array['fheight'], 'left', 'normal', 0.10, '000000');
						fxpdf_text($pdf, $data['sb'], $x4, $y, $pdf_array['fheight'], 'left', 'normal', 0.01, '000000');
						fxpdf_text($pdf, '(= '.$weekdays[getWeekday($data['sbd'])].', '.get_entry($data['sbd'], "datum").')', -3.0, $y, $pdf_array['fheight'], 'right', 'normal', 0.01, '000000');
						$y += 1.50*$pdf_array['ffheight'];

						// ...SE=spätestes Ende (in Tagen nach Projektstart)
						fxpdf_text($pdf, $leg_arr[8]['dl'], $x3, $y, $pdf_array['fheight'], 'left', 'bold', 0.20, '000000');
						fxpdf_text($pdf, $leg_arr[8]['dt'], $x3+15.0, $y, $pdf_array['fheight'], 'left', 'normal', 0.10, '000000');
						fxpdf_text($pdf, $data['se'], $x4, $y, $pdf_array['fheight'], 'left', 'normal', 0.01, '000000');
						fxpdf_text($pdf, '(= '.$weekdays[getWeekday($data['sed'])].', '.get_entry($data['sed'], "datum").')', -3.0, $y, $pdf_array['fheight'], 'right', 'normal', 0.01, '000000');
						$y += 2.0*$pdf_array['ffheight'];

						if($y < $pdf_array['h_body'])
							$ttda[$vn]=true;
						else
						{
							fxpdf_box($pdf, $x1, $ys, -1.0, $y, $cutcol, '0000', 0.0, 0.0, 0.15, '000000');
							$finished=false;
							break;
						}
					}
				}

				// Testversion
				if($testversion)
					fxpdf_diagonal($pdf, true);

				// FeRox-Fusszeile setzen, falls nicht abgeschaltet
				if(!isset($pdf_array['f_nofoot']))
				{
					$fptext=$txt_append.' '.fxpdf_char($apage);
					fxpdf_clipping($pdf, 2, $fptext, true, $dmb);
				}

				if($finished)
					break;

				fxpdf_page($pdf, 'begin', array($pdf_array['width'], $pdf_array['height']));
				fxpdf_clipping($pdf, 0, false, true);
				$y=1.0;
				$apage++;
			}

			$pdf_array=$pdf_array_backup;
		}

		// Aktuelles PDF-Dokument schliessen
		$pdf_data=fxpdf_page($pdf, 'close');
		$pdf_len=strlen($pdf_data);
	}

	// Einstellungswerte wieder zurücksetzen
	$GLOBALS['fxpglobals']['settings']=$settings_backup;

	if(isset($_preview_config))
	{
		// PDF-Dokument speichern und Redirect zum direkten Anzeigen
		$ad=fx_date('YmdHis').$GLOBALS['fxpglobals']['person'];
		$fs=$GLOBALS['tmppath'].'~'.$ad.'.pdf';
		$fe=fxSaveBin($fs, $pdf_data);
		if(!$fe)
			echo('<head><meta http-equiv="refresh" content="0;URL=loader.php?url=config_preview.inc&pf='.$ad.'"></head>');
	}
	else
	{
		$fn=urlencode($pdf_array['filename']);
		dm_window($fn);
	}
}


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * Linie zeichnen
 *
 * @param various $pdf    - Mandatory parameter:                      ???PARAMETER???
 * @param various $vn     - Mandatory parameter:                      ???PARAMETER???
 * @param string  $lcolor - Optional parameter (default='000000'):  ???PARAMETER???
 * @param double  $lwidth - Optional parameter (default=0.50):      ???PARAMETER???
 */
function fxpdf_nzpLine($pdf, $vn, $lcolor='000000', $lwidth=0.50)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$data=$GLOBALS['fxptdata']['pdf']['prj_info_arr'][$vn];
	if($data['pre'])
	{
		$pvn	= $data['pre'];
		$pdata	= $GLOBALS['fxptdata']['pdf']['prj_info_arr'][$pvn];

		$x1	= $pdata['left'] + $GLOBALS['ne_tgap'] + $GLOBALS['ne_width'] + $GLOBALS['pdf_array']['fac'] - $GLOBALS['xmin'];

		$x2	= $data['left'] + $GLOBALS['ne_tgap'] - $GLOBALS['pdf_array']['fac'] - $GLOBALS['xmin'];
		$y2	= $data['top'] + $GLOBALS['nb_height']/2  - $GLOBALS['ymin'];

		// In gleicher Höhe
		if($data['line'] == $pdata['line'])
		{
			if(fxpdf_is_visible($x1, $y2, $x2, $y2))
				fxpdf_line($pdf, '!'.$x1, '!'.$y2, '!'.$x2, '!'.$y2, 0.0, 0.0, $lwidth, $lcolor);
		}
		// In unterschiedlicher Höhe
		else
		{
			$xm=$x1+(($x2-$x1)/2);
			$y1=$pdata['top'] + $GLOBALS['nb_height']/2 - $GLOBALS['ymin'];

			if(fxpdf_is_visible($x1, $y1, $xm, $y1))
				fxpdf_line($pdf, '!'.$x1, '!'.$y1, '!'.$xm, '!'.$y1, 0.0, 0.0, $lwidth, $lcolor);
			if(fxpdf_is_visible($xm, $y1, $xm, $y2))
				fxpdf_line($pdf, '!'.$xm, '!'.$y1, '!'.$xm, '!'.$y2, 0.0, 0.0, $lwidth, $lcolor);
			if(fxpdf_is_visible($xm, $y2, $x2, $y2))
				fxpdf_line($pdf, '!'.$xm, '!'.$y2, '!'.$x2, '!'.$y2, 0.0, 0.0, $lwidth, $lcolor);
		}

		// Pfeilspitze
		$ah=6.0*$GLOBALS['pdf_array']['fac'];
		$ym=$y2;
		$x2 += $GLOBALS['pdf_array']['fac'];
		$x1=$x2 - $GLOBALS['ne_tgap'];
		$y1=$ym - $ah/2;
		$y2=$ym + $ah/2;
		if(fxpdf_is_visible($x1, $y1, $x2, $y2))
		{
			fxpdf_linestyle($pdf, 0.0, 0.0, $lwidth);
			fxpdf_color($pdf, 'S', 0, $lcolor);
			fxpdf_color($pdf, 'F', 32, $lcolor);
			$pdf->Arrow(fxpdf_getx('!'.$x1), fxpdf_gety('!'.$ym), fxpdf_getx('!'.$x2), fxpdf_gety('!'.$ym), 2, ((($GLOBALS['ne_tgap']*3)/4)*$GLOBALS['pdf_array']['fac']), 18);
		}
	}
}
?>