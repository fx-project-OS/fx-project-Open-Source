<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : d_manager_psp.inc                                            //
// Version     : 21.1                                                         //
// Begin       : 2021-01-11                                                   //
// Last Change : 2021-01-11                                                   //
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
 * PF 84: Project Structure Plan (PSP)
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

	$txt_append=meldung(1588, false);						// Anhang
	$txt_open='{'.meldung(1794, false).'}';					// offen
	$txt_nstart='{'.meldung(2833, false).'}';				// nocht nicht begonnen
	$txt_psp=meldung(2575, false);							// Projektstrukturplan (PSP)
	$txt_lit=explode('|', meldung(2831, false));			// Vorgangsart|Status|Priorität|Planungszeitraum (Soll)|Arbeitszeitraum (Ist)|Aktionsnachricht|Int. Budgets (verbr./gen.)|Aufwand (vebr./gepl.)

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

	// Konfigurationsfile einladen und parsen: -2=cfg_ppsp.cfg (Projektstrukturplan)
	$pdf_array=load_pdfconfig(-2);
	// ...Zusatzinfos zum Array
	$pdf_array['filename']='fx_psp_pdf';																		// Filename der zu erzeugenden PDF-Datei
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
//fxDebug($pdf_array, '$pdf_array');

	// PSP-Variablen
	$top_vn='';
	$bgarray=array('ok'=>"99ff99", 'wn'=>"ffff99", 'ko'=>"ff5c5c", 'in'=>"cccccc");

	while(true)
	{
		$pdf_array['ffheight']=$pdf_array['fheight']*$GLOBALS['pdf_array']['fac'];

		$ewidth		= 86.0*$pdf_array['fac'];
		$ewidth2	= round($ewidth/2, 2);
		$eheight	= 50.0*$pdf_array['fac'];
		$eheight2	= round($eheight/2, 2);
		$egap		= 11.0*$pdf_array['fac'];
		$egap2		= round($egap/2, 2);
		$edist		= 17.0*$pdf_array['fac'];

		$bpadding	= 0.0; //12.0*$pdf_array['fac'];
		$bwidth		= 2.0*$bpadding + $GLOBALS['fxptdata']['pdf']['prj_info_arr']['var']['columns']*$ewidth  + ($GLOBALS['fxptdata']['pdf']['prj_info_arr']['var']['columns']-1)*$edist + $GLOBALS['fxptdata']['pdf']['prj_info_arr']['var']['gaps']*$egap + 2.0*$pdf_array['fac'];
		$bheight	= 2.0*$bpadding + ($GLOBALS['fxptdata']['pdf']['prj_info_arr']['var']['lines']+1)*$eheight  + $GLOBALS['fxptdata']['pdf']['prj_info_arr']['var']['lines']*$egap + 4.0*$pdf_array['fac'];
//echo('$bwidth='.$bwidth.', $bheight='.$bheight.'<br>');

		if($pdf_array['fit'] && ($bwidth > $pdf_array['w_body']))
		{
			$nf=floor($pdf_array['w_body'])/ceil($bwidth);
			$pdf_array['fac']=$nf;
		}
		else
			break;
	}

	// Anzahl Seiten und Subseiten ermitteln
	$pyoffset=50.0;

	// Legende
	$lfac		= 0.75*$GLOBALS['pdf_array']['fac']; // Faktor für Legende
	$lwidth		= 0.0;
	$lheight	= 0.0;
	if($pdf_array['leg'])	// Legende anzeigen?
	{
		$lwidth		= 420.0*$lfac;
		$lheight	= 50.0*$lfac;

		// Evtl. Faktor verkleinern, falls Breite nicht auf Seite passt
		if($lwidth < $pdf_array['h_width'])
		{
			$lfac		= $pdf_array['h_width']/$lwidth;
			$lwidth		= 420.0*$lfac;
			$lheight	= 50.0*$lfac;
		}
	}

	$pages=1;
	$pheight=$pdf_array['h_body'];
	$theight=$pyoffset+$bheight+$lheight;
//		if($lheight)
//			$theight += 20.0;
	while($pheight < $theight)
	{
		$pages++;
		$pheight += $pdf_array['h_body'];
	}
	$subpages=0;
	$pwidth=$pdf_array['w_body'];
	while($pwidth < $bwidth)
	{
		$subpages++;
		$pwidth += $pdf_array['w_body'];
	}
	$pxoffset=round(($pwidth-$bwidth)/2, 2);
//echo('$pages='.$pages.', $subpages='.$subpages.' -- $pheight='.$pheight.', $pwidth='.$pwidth.', -- $pyoffset='.$pyoffset.', $pxoffset='.$pxoffset.'<br>'.$nl);

	// Koordinaten festlegen
	foreach($GLOBALS['fxptdata']['pdf']['prj_info_arr'] as $vn => $data)
	{
		if($vn != 'var')
		{
			if(!strlen($top_vn))
				$top_vn=$vn;

			$etop=$bpadding + $data['line']*($eheight+$egap);

			$lcolumn=$data['lcolumn'];
			$rcolumn=$data['rcolumn'];
			$dcolumns=$rcolumn+1-$lcolumn;
			if((!$GLOBALS['fxptdata']['pdf']['prj_info_arr']['var']['dmode'] && !$data['line']) || ($GLOBALS['fxptdata']['pdf']['prj_info_arr']['var']['dmode'] && !$data['dtasks'] && ($dcolumns > 1)))
			{
				$cc=$lcolumn+floor($dcolumns/2);
				$gap=$data['gap'];
				for($c=0; $c<=$cc; $c++)
					$gap += $GLOBALS['fxptdata']['pdf']['prj_info_arr']['var']['gpc'][$c];
				$crl=$bpadding + $cc*($ewidth+$edist) + $gap*$egap;

				if(!($dcolumns%2))
				{
					$cc--;
					$cll=$bpadding + $cc*($ewidth+$edist) + $gap*$egap;

					$eleft=$cll + floor(($crl-$cll)/2);
				}
				else
					$eleft=$crl;
			}
			else
			{
				$gap=$data['gap'];
				for($c=0; $c<=$lcolumn; $c++)
					$gap += $GLOBALS['fxptdata']['pdf']['prj_info_arr']['var']['gpc'][$c];
				$eleft=$bpadding + $lcolumn*($ewidth+$edist) + $gap*$egap;
			}

			$GLOBALS['fxptdata']['pdf']['prj_info_arr'][$vn]['top']=$pyoffset+$etop;
			$GLOBALS['fxptdata']['pdf']['prj_info_arr'][$vn]['left']=$pxoffset+$eleft;
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
//echo('$page='.$page.', $subpage='.$subpage.' -- $ymin='.$ymin.', $ymax='.$ymax.', $xmin='.$xmin.', $xmax='.$xmax.'<br>'.$nl);

				// Headline
				if($page == 1)
				{
					// ...Projektstrukturplan (PSP)
					fxpdf_font($pdf, -16.0);
					$lpt=$pdf->GetStringWidth($txt_psp);
					if(8.0+$lpt > $pwidth-16.0)
					{
						$tw=fxpdf_wraptext($pdf, $txt_psp, -16.0, 'B', $pwidth-28.0);
						$txt_psp=trim($tw[0]).'...';
						$lpt=$pdf->GetStringWidth($txt_psp);
					}
					fxpdf_text($pdf, $txt_psp, '!'.(8.0-$xmin), 20.0, -16.0, 'left', 'bold', -0.25, '000000');

					// ...Vorgangsnummer + -name
					$xa=40.0;
					$xv=$lpt+$xa;
					if($xv < $pwidth-$xa)
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
						$y2=$y1+$eheight;
						$x1=$data['left']-$xmin;
						$x2=$x1+$ewidth;
						// Element im Anzeigebereich?
						// ...JA, Element ist im Anzeigebereich
						if(fxpdf_is_visible($x1, $y1, $x2, $y2))
						{
//echo('<font color="#008800">'.$vn.'</font>: $x1='.$x1.', $y1='.$y1.' - $x2='.$x2.', $y2='.$y2.'<br>'.$nl);

							// Shadow
							if(!$data['type'])
							{
								$soffset=2.0*$pdf_array['fac'];
								fxpdf_box($pdf, '!'.($x1+$soffset), '!'.($y1+$soffset), '!'.($x2+$soffset), '!'.($y2+$soffset), 'bbbbbb', '0000');
							}

							// Background
							if(strlen($data['disp']['image']))
								fxpdf_gbox($pdf, '!'.$x1, '!'.$y1, '!'.$x2, '!'.$y2, 'ffffff', $bgarray[$data['disp']['image']]);
							else
								fxpdf_box($pdf, '!'.$x1, '!'.$y1, '!'.$x2, '!'.$y2, 'ffffff', '0000');

							// Vorgang gestartet, dann Linie von links unten nach rechts oben
							if(strlen($data['disp']['image-add']))
							{
								fxpdf_line($pdf, '!'.$x1, '!'.$y2, '!'.$x2, '!'.$y1, 0.0, 0.0, 0.40, 'a8a8a8');
								// Vorgang abgeschlossen, dann gekreuzte Linien
								if($data['disp']['image-add'] == "_f")
									fxpdf_line($pdf, '!'.$x1, '!'.$y1, '!'.$x2, '!'.$y2, 0.0, 0.0, 0.40, 'a8a8a8');
							}

							// Border
							$wd=0.0;
							$bd=0.0;
							if($data['disp']['border-style'] == "dotted")
							{
								$wd=1.0;
								$bd=1.0;
								if(strlen($GLOBALS['fxptdata']['pdf']['prj_info_arr']['var']['cp_vn']) && $data['cp'])
								{
									$wd=0.50;
									$bd=0.50;
								}
							}
							fxpdf_rec($pdf, '!'.$x1, '!'.$y1, '!'.$x2, '!'.$y2, $wd, $bd, 0.40, $data['disp']['border-color']);

							// Vorgangsname
							$txt=fxpdf_wraptext($pdf, strtr($data['name'], $trans_html), $pdf_array['fheight'], 'B', ($x2-$x1)*0.90);
							$x=$x1+($x2-$x1)/2;
							$l=$y1+2.50*$pdf_array['ffheight'];
							$s=1;
							$y=$l+($y2-$l)/2;
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
							fxpdf_text($pdf, $data['nr'], '!'.$x, '!'.($y1+$pdf_array['ffheight']*1.50), $pdf_array['fheight'], 'center', 'bold', 0.50, '000000');
							for($c=0; $c<$s; $c++)
							{
								fxpdf_text($pdf, $txt[$c], '!'.$x, '!'.$y, $pdf_array['fheight'], 'center', 'normal', 0.25, '333333');
								$y += $pdf_array['ffheight'];
							}
						}
						// ...NEIN, Element ist nicht im Anzeigebereich
						else
						{
//echo('<font color="#880000">'.$vn.'</font>: $x1='.$x1.', $y1='.$y1.' - $x2='.$x2.', $y2='.$y2.'<br>'.$nl);
						}
					}
				}

				// Linien zum übergeordneten Vorgang
				foreach($GLOBALS['fxptdata']['pdf']['prj_info_arr'] as $vn => $data)
				{
					if(($vn != 'var') && (!strlen($GLOBALS['fxptdata']['pdf']['prj_info_arr']['var']['cp_vn']) || !$data['cp']))
						fxpdf_pspLine($pdf, $vn, 'a8a8a8');
				}
				// Linie des kritischen Pfades zum übergeordnten Vorgang
				foreach($GLOBALS['fxptdata']['pdf']['prj_info_arr'] as $vn => $data)
				{
					if(($vn != 'var') && strlen($GLOBALS['fxptdata']['pdf']['prj_info_arr']['var']['cp_vn']) && $data['cp'])
						fxpdf_pspLine($pdf, $vn, 'cc4444');
				}

				// Legende auf allerletzter Seite/Subseite
				if($lheight && $lwidth && ($page == $pages) && ($subpage == $subpages))
				{
					$rfac							= $GLOBALS['pdf_array']['fac'];
					$GLOBALS['pdf_array']['fac']	= $lfac;

					$txt_ll		= explode('|', meldung(2836, false));	// Farbe|Rahmen
					$txt_color	= explode('|', meldung(2837, false));	// Zukunft gepl.|Inakt./Abgeschl.|Im Plan|Hinweis prüfen|Vorgang anpassen
					$txt_frame	= explode('|', meldung(2838, false));	// Aktiv/Abgeschl.|Inakt./Geplant|Begonnen|Abgeschlossen|Krit. Pfad
					$lc_array	= array('ft', 'in', 'ok', 'wn', 'ko');

					$ltwidth =55.0*$lfac;
					$llwidth =35.0*$lfac;
					$lewidth =20.0*$lfac;
					$leheight=10.0*$lfac;

					$y=$pdf_array['h_body'] - $lheight;
					$x=$pdf_array['h_width'] - $lwidth;
					fxpdf_rec($pdf, $x, $y, '-0', '-0', 2.0, 2.0, 0.25, 'bbbbbb');

					$xt=$x+4.0*$lfac;
					$yt=$y+12.0*$lfac;
					// Legende:
					fxpdf_text($pdf, meldung(1994, false), $xt, $yt, 8.0, 'left', 'bold', 0.25, '444444');

					$yt += 3.0*$lfac;
					fxpdf_line($pdf, $xt, $yt, '-'.(4.0*$lfac), $yt, 0.0, 0.0, 0.25, 'bbbbbb');

					$xt =$x+4.0*$lfac;
					$yt += 5.0*$lfac;
					// Farbe:
					fxpdf_text($pdf, $txt_ll[0].':', $xt, $yt+$leheight-2.0*$lfac, 5.0, 'left', 'bold', 0.25, '666666');

					foreach($lc_array as $lcnt => $lsym)
					{
						if(!$lcnt)	// Sonderfall: 0=Zukunft gepl.
						{
							$xt += $llwidth;
							fxpdf_box($pdf, $xt, $yt, $xt+$lewdith, $yt+$leheight, 'ffffff', '0000');
						}
						else
						{
							$xt += $lewidth+$ltwidth;
							fxpdf_gbox($pdf, $xt, $yt, $xt+$lewidth, $yt+$leheight, 'ffffff', $bgarray[$lsym]);
						}
						fxpdf_rec($pdf, $xt, $yt, $xt+$lewidth, $yt+$leheight, 0.0, 0.0, 0.25, 'bbbbbb');
						fxpdf_text($pdf, $txt_color[$lcnt], $xt+$lewidth+4.0*$lfac, $yt+$leheight-2.0*$lfac, 5.0, 'left', 'normal', 0.25, '999999');
					}

					$xt =$x+4.0*$lfac;
					$yt += $leheight+4.0*$lfac;
					// Rahmen:
					fxpdf_text($pdf, $txt_ll[1].':', $xt, $yt+$leheight-2.0*$lfac, 5.0, 'left', 'bold', 0.25, '666666');
					// ...Aktiv/Abgeschl.
					$xt += $llwidth;
					fxpdf_rec($pdf, $xt, $yt, $xt+$lewidth, $yt+$leheight, 0.0, 0.0, 0.25, 'bbbbbb');
					// ...Inakt./Geplant
					$xt += $lewidth+$ltwidth;
					fxpdf_rec($pdf, $xt, $yt, $xt+$lewidth, $yt+$leheight, 1.0, 1.0, 0.25, 'bbbbbb');
					// ...Begonnen
					$xt += $lewidth+$ltwidth;
					fxpdf_line($pdf, $xt, $yt+$leheight, $xt+$lewidth, $yt, 0.0, 0.0, 0.25, 'bbbbbb');
					fxpdf_rec($pdf, $xt, $yt, $xt+$lewidth, $yt+$leheight, 0.0, 0.0, 0.25, 'bbbbbb');
					// ...Abgeschlossen
					$xt += $lewidth+$ltwidth;
					fxpdf_line($pdf, $xt, $yt, $xt+$lewidth, $yt+$leheight, 0.0, 0.0, 0.25, 'bbbbbb');
					fxpdf_line($pdf, $xt, $yt+$leheight, $xt+$lewidth, $yt, 0.0, 0.0, 0.25, 'bbbbbb');
					fxpdf_rec($pdf, $xt, $yt, $xt+$lewidth, $yt+$leheight, 0.0, 0.0, 0.25, 'bbbbbb');
					// ...Krit. Pfad
					$xt += $lewidth+$ltwidth;
					fxpdf_line($pdf, $xt+$lewidth/2, $yt, $xt+$lewidth/2, $yt+$leheight/2, 0.0, 0.0, 0.25, 'cc4444');
					fxpdf_line($pdf, $xt+$lewidth/2, $yt+$leheight/2, $xt+$lewidth, $yt+$leheight/2, 0.0, 0.0, 0.25, 'cc4444');
					// ...Texte
					$xt=$x+4.0*$lfac;
					foreach($txt_frame as $lcnt => $ltxt)
					{
						if(!$lcnt)	// Sonderfall
							$xt += $llwidth;
						else
							$xt += $lewidth+$ltwidth;
						fxpdf_text($pdf, $ltxt, $xt+$lewidth+4.0*$lfac, $yt+$leheight-2.0*$lfac, 5.0, 'left', 'normal', 0.25, '999999');
					}

					$GLOBALS['pdf_array']['fac']	= $rfac;
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
			$cwidth	= 426.0;
			$x1		= 1.0;
			$x2		= $pdf_array['w_body']-8.0-$cwidth;
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
						$vt=fxpdf_wraptext($pdf, $data['nr'].': '.strtr($data['name'], $trans_html), $pdf_array['fheight']*1.25, 'B', $pdf_array['w_body']-40.0);
						$txt=$vt[0];
						if(sizeof($vt) > 1)
							$txt .= '...';
						fxpdf_text($pdf, $txt, $x1+18.0, $y+$pdf_array['ffheight']*1.50, $pdf_array['fheight']*1.25, 'left', 'bold', 0.25, '000066');
						// ...Ampelfarbe
						if(strlen($data['disp']['image']))
							fxpdf_gbox($pdf, $x1, $y, $x1+16.0, $y+2.0*$pdf_array['ffheight'], 'ffffff', $bgarray[$data['disp']['image']]);
						else
							fxpdf_box($pdf, $x1, $y, $x1+16.0, $y+2.0*$pdf_array['ffheight'], 'ffffff');
						$y += 3.50*$pdf_array['ffheight'];

						// ...Vorgangsart
						fxpdf_text($pdf, $txt_lit[0].':', $x1+2.0, $y, $pdf_array['fheight'], 'left', 'bold', 0.20, '000000');
						fxpdf_text($pdf, $type_array[$data['ptype']], $x2+4.0, $y, $pdf_array['fheight'], 'left', 'normal', 0.01, '000000');
						$y += 1.50*$pdf_array['ffheight'];

						// ...Status
						fxpdf_text($pdf, $txt_lit[1].':', $x1+2.0, $y, $pdf_array['fheight'], 'left', 'bold', 0.20, '000000');
						fxpdf_text($pdf, $stat_array[$data['stat']], $x2+4.0, $y, $pdf_array['fheight'], 'left', 'normal', 0.01, '000000');
						$y += 1.50*$pdf_array['ffheight'];

						// ...Priorität
						fxpdf_text($pdf, $txt_lit[2].':', $x1+2.0, $y, $pdf_array['fheight'], 'left', 'bold', 0.20, '000000');
						fxpdf_text($pdf, $prio_array[$data['prio']], $x2+4.0, $y, $pdf_array['fheight'], 'left', 'normal', 0.01, '000000');
						$y += 1.50*$pdf_array['ffheight'];

						// ...Planungszeitraum (Soll)
						fxpdf_text($pdf, $txt_lit[3].':', $x1+2.0, $y, $pdf_array['fheight'], 'left', 'bold', 0.20, '000000');
						fxpdf_text($pdf, get_entry($data['ps'], "datum").' - '.get_entry($data['pe'], "datum"), $x2+4.0, $y, $pdf_array['fheight'], 'left', 'normal', 0.01, '000000');
						$y += 1.50*$pdf_array['ffheight'];

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
						$y += 1.50*$pdf_array['ffheight'];

						// ...Int. Budgets (verbr./gen.)
						if(isset($data['bapp']) && isset($data['buse']))
						{
							fxpdf_text($pdf, $txt_lit[6].':', $x1+2.0, $y, $pdf_array['fheight'], 'left', 'bold', 0.20, '000000');
							fxpdf_text($pdf, get_entry($data['buse'], "dezimal").' '.$GLOBALS['fxpglobals']['settings']['waehrung'].' / '.get_entry($data['bapp'], "dezimal").' '.$GLOBALS['fxpglobals']['settings']['waehrung'], $x2+4.0, $y, $pdf_array['fheight'], 'left', 'normal', 0.01, '000000');
							$y += 1.50*$pdf_array['ffheight'];
						}

						// ...Aufwand (vebr./gepl.)
						fxpdf_text($pdf, $txt_lit[7].':', $x1+2.0, $y, $pdf_array['fheight'], 'left', 'bold', 0.20, '000000');
						fxpdf_text($pdf, get_entry($data['effa'], "dezimal").' h / '.get_entry($data['effp'], "dezimal").' h', $x2+4.0, $y, $pdf_array['fheight'], 'left', 'normal', 0.01, '000000');
						$y += 2.50*$pdf_array['ffheight'];

						// ...Aktionsnachricht
						fxpdf_text($pdf, $txt_lit[5].':', $x1+2.0, $y, $pdf_array['fheight'], 'left', 'bold', 0.20, '000000');
						$y -= 1.50*$pdf_array['ffheight'];
						fxpdf_color($pdf, 'T', 0, '000000');
						fxpdf_color($pdf, 'S', 0, 'bbbbbb');
						$pdf->writeHTMLCell($cwidth, 0, fxpdf_getx($x2+1.65), fxpdf_gety($y), trim(strtr($data['tt'], $trans_html)), 0, 0);
						$y += substr_count($data['tt'], '<br')*$pdf_array['ffheight'] + (2.0+substr_count($data['tt'], '<hr')*2.0)*$pdf_array['ffheight'];

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
 */
function fxpdf_pspLine($pdf, $vn, $lcolor='000000')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$data=$GLOBALS['fxptdata']['pdf']['prj_info_arr'][$vn];
	if($data['line'] > 0)
	{
		$pvn=$data['parent'];

		// ...Vom Teilprojekt zum übergeordneten (Teil)Projekt
		if((!$GLOBALS['fxptdata']['pdf']['prj_info_arr']['var']['dmode'] && ($data['line'] == 1)) || ($GLOBALS['fxptdata']['pdf']['prj_info_arr']['var']['dmode'] && !$data['type']))
		{
			$x1=$GLOBALS['fxptdata']['pdf']['prj_info_arr'][$pvn]['left'] + $GLOBALS['ewidth2'] - $GLOBALS['xmin'];
			$y1=$GLOBALS['fxptdata']['pdf']['prj_info_arr'][$pvn]['top'] + $GLOBALS['eheight'] - $GLOBALS['ymin'] + 0.40*$GLOBALS['pdf_array']['fac'];
			$y2=$data['top'] - $GLOBALS['ymin'] - 0.40*$GLOBALS['pdf_array']['fac'];

			if($GLOBALS['fxptdata']['pdf']['prj_info_arr'][$pvn]['left'] == $data['left'])
			{
				if(fxpdf_is_visible($x1, $y1, $x1, $y2))
					fxpdf_line($pdf, '!'.$x1, '!'.$y2, '!'.$x1, '!'.$y1, 0.0, 0.0, 0.40, $lcolor);
			}
			else
			{
				$x2=$data['left'] + $GLOBALS['ewidth2'] - $GLOBALS['xmin'];
				$ym=$y1 + round(($y2-$y1)/2, 2);

				if(fxpdf_is_visible($x2, $ym, $x2, $y2))
					fxpdf_line($pdf, '!'.$x2, '!'.$ym, '!'.$x2, '!'.$y2, 0.0, 0.0, 0.40, $lcolor);
				if(fxpdf_is_visible($x1, $ym, $x2, $ym))
					fxpdf_line($pdf, '!'.$x1, '!'.$ym, '!'.$x2, '!'.$ym, 0.0, 0.0, 0.40, $lcolor);
				if(fxpdf_is_visible($x1, $y1, $x1, $ym))
					fxpdf_line($pdf, '!'.$x1, '!'.$y1, '!'.$x1, '!'.$ym, 0.0, 0.0, 0.40, $lcolor);
			}
		}
		// ...zum Elter
		else
		{
			$x1=$GLOBALS['fxptdata']['pdf']['prj_info_arr'][$pvn]['left'] + $GLOBALS['egap2'] - $GLOBALS['xmin'];
			$y1=$GLOBALS['fxptdata']['pdf']['prj_info_arr'][$pvn]['top'] + $GLOBALS['eheight'] - $GLOBALS['ymin'] + 0.40*$GLOBALS['pdf_array']['fac'];
			$x2=$data['left'] - $GLOBALS['xmin'] - 0.40*$GLOBALS['pdf_array']['fac'];
			$y2=$data['top'] + $GLOBALS['eheight2'] - $GLOBALS['ymin'];

			if(fxpdf_is_visible($x1, $y1, $x1, $y2))
				fxpdf_line($pdf, '!'.$x1, '!'.$y1, '!'.$x1, '!'.$y2, 0.0, 0.0, 0.40, $lcolor);
			if(fxpdf_is_visible($x1, $y2, $x2, $y2))
				fxpdf_line($pdf, '!'.$x1, '!'.$y2, '!'.$x2, '!'.$y2, 0.0, 0.0, 0.40, $lcolor);
		}
	}
}
?>