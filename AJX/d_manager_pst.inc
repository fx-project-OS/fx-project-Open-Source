<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : d_manager_pst.inc                                            //
// Version     : 21.2                                                         //
// Begin       : 2020-12-10                                                   //
// Last Change : 2020-12-10                                                   //
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
 * AJAX: File Manager for Calendar Diagrams
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

$rd_debug=0;
$pdf_cal_info_debug=0;
$pdf_function_debug=0;	// Alle PDF-Funktionen anzeigen

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
$GLOBALS['_ext_pdf_pst']=true;
$req_fn=fxf_fn_reqFilename('ext_pdf');
if(strlen($req_fn))
	require($req_fn);

$_backup_tr=$GLOBALS['fxpglobals']['tr'];
$_backup_lang=$GLOBALS['fxpglobals']['lang'];
if(isset($_preview_tr))
{
	$tr=$_preview_tr;
	$lang=$lng;
}
else
{
	$tr=$_backup_tr;
	$lang=$_backup_lang;
}
//echo('$tr='.$tr.'<br />');
$GLOBALS['fxpglobals']['tr']=$tr;
$GLOBALS['fxpglobals']['lang']=$lang;

if($rd_debug)
	fxDebug('_POST');

if(isset($_preview_cal) && is_array($_preview_cal))
	$GLOBALS['fxptdata']['cal']=$_preview_cal;

// Projekt-Display-Array erstellen
$GLOBALS['pda']=array();
if(fxIsArray($GLOBALS['fxptdata']) && fxIsArray($GLOBALS['fxptdata']['_fxform_selmask_1']))
{
//fxDebug($GLOBALS['fxptdata']['_fxform_selmask_1'], '_fxform_selmask_1');
	foreach($GLOBALS['fxptdata']['_fxform_selmask_1'] as $sk => $sv)
	{
		if(substr($sk,0,4) == 'disp')
		{
			$pid=(int)substr($sk,4);
			if(substr($sv,0,1) == '_')			// Display: "_cl" (close icon - folder open), "_op" (open icon - folder closed) or "_" (no icon - task)
				$GLOBALS['pda'][$pid]=true;
			else								// Hidden: "none_"
				$GLOBALS['pda'][$pid]=false;
		}
	}
}
//fxDebug($GLOBALS['pda'], 'pda');

//fxDebug($GLOBALS['fxptdata']['cal'], 'cal');
if(fxIsArray($GLOBALS['fxptdata']['cal']))
{
	$ad=fx_date('YmdHis').$GLOBALS['fxpglobals']['person'];
	$trans_html=array_flip(get_html_translation_table(HTML_ENTITIES));
	$trans_html['&nbsp;']=chr(32);
	$trans_html['&middot;']='·';

	$pscal=$GLOBALS['fxptdata']['cal'];
	$dmb='';
	$title='';

	// Style wegen Farbinfos einladen
	$cssnr=$GLOBALS['cssfile'];
	$style_array=@file($cssnr);
	$style=array();
	if(fxIsArray($style_array))
	{
		foreach($style_array as $l)
		{
			$l=strtolower(trim($l));
			if((substr($l, 0, 1) == '#') || (substr($l, 0, 1) == '.'))
			{
				$bp=strpos($l,'{');
				if($bp !== false)
				{
					$vsa=explode(',',trim(substr($l,0,$bp)));
					$ds=substr($l,$bp);
					$cf='';
					$sp=0;
					while(true)
					{
						$cp=strpos($ds,'background',$sp);
						if($cp === false)
							break;
						$sp=$cp+10;
						$cp=strpos($ds,':',$sp);
						if($cp === false)
							break;
						$sp=$cp+1;
						$cp=strpos($ds,';',$sp);
						if($cp === false)
							break;
						$cs=substr($ds,$sp,$cp-$sp);
						$sp=$cp+1;
						$hp=strpos($cs,'#');
						if($hp !== false)
						{
							$bg=substr($cs,$hp+1,6);
							foreach($vsa as $v)
							{
								$cp=strpos($v,':');
								if($cp === false)
								{
									$v=substr(trim($v),1);
									$dp=strpos($v,' ');
									if($dp !== false)
										$v=trim(substr($v,0,$dp));
									if(strlen($v) && !isset($style[$v]))
										$style[$v]=$bg;
								}
							}
						}
					}
				}
			}
		}
	}
//fxDebug($style, 'style');

	// Skalierungsfaktor für Grafiken
	$pdf_picscale=0.65;

	// Einstellungswerte für Druckaufbereitung verwenden
	$settings_backup=$GLOBALS['fxpglobals']['settings'];
	$GLOBALS['fxpglobals']['settings']['formatdatum']=$GLOBALS['fxpglobals']['settings']['d_formatdatum'];
	$GLOBALS['fxpglobals']['settings']['formatzeit']=$GLOBALS['fxpglobals']['settings']['d_formatzeit'];
	$GLOBALS['fxpglobals']['settings']['deztrennzeichen']=$GLOBALS['fxpglobals']['settings']['d_deztrennzeich'];
	$GLOBALS['fxpglobals']['settings']['nkstellen']=$GLOBALS['fxpglobals']['settings']['d_nkstellen'];

	// Cal-Array-Infos ermitteln
	$GLOBALS['ci']=array();
	$pdf=false;
	dpdf_cal_info($pdf);
//fxDebug($ci, __LINE__.': $ci');

	// Zusatzinfos zum Array
	$pdf_array['filename']='fx_pdf';														// Filename der zu erzeugenden PDF-Datei
	$pdf_array['h_pfoot']=(float)16.0;														// Höhe der FeRox-Fusszeile
	$pdf_array['min_y']=$pdf_array['tmargin'];												// Obere Begrenzung
	$pdf_array['max_y']=$pdf_array['bmargin'];												// Untere Begrenzung
	$pdf_array['w_body']=$pdf_array['width']-$pdf_array['lmargin']-$pdf_array['rmargin'];	// Innere Breite

	if(isset($pdf_array['f_nofoot']))
		$pdf_array['h_pfoot']=(float)0.0;

	if($rd_debug)
		fxDebug($pdf_array, '$pdf_array');

	// Ein neues PDF-Objekt erstellen mit Standard-Fehlerhandling und -Speicherverwaltung.
	$pdf=false;
	if(strlen($GLOBALS['_ext_pdf']))
		$pdf=fxpdf_new();
//fxDebug($pdf,'$pdf', 0);
	if($pdf)
	{
		// PDF-Dokument öffnen und Infofelder setzen
		fxpdf_page($pdf,'open');
		// Fusszeileninfo ermitteln
		$dmb=fxpdf_foot_info();

		$cc=0;	// Spaltenzähler
		$cut=0;	// Aktuelle Subseite

		$emc=0;	// Emergency counter and break;
		$eme=($ci['lcol']+$ci['lcol'])*$ci['lines'];

		// 2879:Abwesend,  1178+1039:Urlaub,  2877+2878:Sonderurlaub,  2106+1468:Schulung,  2105+1040:Krank,  1940+1941:Gleittag
		$starr=array(2879=>false,  1178=>true,1039=>false,  2877=>true,2878=>false,  2106=>true,1468=>false,  2105=>true,1040=>false,  1940=>true,1941=>false);
		$sttarr=array(2879=>2880,  1178=>1245,1039=>767,  2877=>2881,2878=>2882,  2106=>2005,1468=>2107,  2105=>768,1040=>2108,  1940=>2385,1941=>2407);

//$max_cc=1;
		$max_cc=$ci['rcol']-1;
		$tyc0=-1.0;
		$tyc1=-1.0;
//echo('<div style="margin-top:8px;padding:8px;border:1px solid #000000;background:#ff88ff;">$ci[rcol]='.$ci['rcol'].' ($cc='.$cc.' <= $max_cc='.$max_cc.')</div>'.$GLOBALS['nl']);
		while($ci['rcol'] && ($cc <= $max_cc))	// Spalten durchgehen
		{
			$acc=$ci['lcol']+$cc;	// Aktuelle Spalte
			$lc=0;					// Zeilenzähler
			$seite=1;				// Aktuelle Seite
//echo('<div style="margin-top:8px;padding:8px;border:1px solid #000000;background:#ffffff;">SPALTE $acc='.$acc.' = ($ci[lcol]='.$ci['lcol'].' + $cc='.$cc.')</div>'.$GLOBALS['nl']);

//$max_lc=1;
			$max_lc=$ci['lines'];
			while($max_lc && ($lc < $max_lc))	// Zeilen durchgehen
			{
//echo('<div style="margin-top:8px;padding:8px;border:1px solid #000000;background:#ffff00;">SEITE $seite='.$seite.' -- SPALTE $acc='.$acc.', ZEILE $lc='.$lc.'</div>'.$GLOBALS['nl']);

				// Neue Seite im vordefinierten Format erstellen.
				fxpdf_page($pdf,'begin');

				// Spaltenbreiten + -höhe
				dpdf_cal_info($pdf);
//fxDebug($ci, __LINE__.': $ci');

				// Body
				fxpdf_clipping($pdf, 0, true, false);
				$fh=max($GLOBALS['pdf_array']['fsize'],$GLOBALS['pdf_array']['fheight']);
				$mf=max(1.0,$GLOBALS['pdf_array']['fac']);
				$lfs=$fh*1.5;
				$tfs=$fh*2.0;
				$cw=6.0*$mf;
				$sf_100=1.00*$mf;
				$sf_050=0.50*$mf;
				$sf_025=0.25*$mf;
				$yc=0.0;
				if(($ci['tr'] == 26) && !$lc)		// Sonderfall: Titel Projektstatus
				{
					if($pdf_array['leg'] == '1')
					{
						$yc=$fh*13.0;
						if(!$GLOBALS['fxptdata']['cal'][0][0]['mst_cnt'])
							$yc -= $fh;
						if(!$GLOBALS['fxptdata']['cal'][0][0]['dep_cnt'])
							$yc -= $fh;
					}
					else
						$yc=$fh*5.0;
					if(!$cc)
					{
						$txt=fxpdf_strlen($pdf, trim($GLOBALS['fxptdata']['cal'][0][0]['title']), -$tfs, 'B', $pdf_array['w_body']);
						if(strlen($txt))
						{
							$title=$txt;
							fxpdf_text($pdf, $txt, 0.0, $tfs, -$tfs, 'left', 'bold');
						}

						// LEGENDE
						if($pdf_array['leg'] == '1')
						{
							// 1388: Projekte, 1475: Aufgaben, 375: Meilensteine, 1587: Abhängigkeiten
							$larr=array(0=>meldung(1388).':', meldung(1475).':');
							if($GLOBALS['fxptdata']['cal'][0][0]['mst_cnt'])
								$larr[2]=meldung(375).':';
							if($GLOBALS['fxptdata']['cal'][0][0]['dep_cnt'])
								$larr[3]=meldung(1587).':';
							// 297: geplant, 299: inaktiv, 300: aktiv, 301: abgeschl.
							$sarr=array(297=>'psgep', 299=>'psina', 300=>'psakt', 301=>'psabg');

							$ccw=$cw*1.5;

							$yl=$fh*5.0;
							$txt=meldung(1994);	// Legende:
							$tw=fxpdf_strlen($pdf, $txt, -$lfs, 'B');
							fxpdf_text($pdf, $txt, 0.0, $yl, -$lfs, 'left', 'bold');
							$yl += 1.0*$mf;
							fxpdf_line($pdf, 0.0, $yl, $tw, $yl, 0.0, 0.0, -$sf_050, '000000');

							$lh=$fh*0.75;
							$yl += $fh*1.4;

							// Projekte/Aufgabe
							$maxw=0.0;
							$ylb=$yl;
							foreach($larr as $lct => $la)
							{
								if($lct)
									$yl += ($fh*1.3);

								$tw=fxpdf_strlen($pdf, $la, -$fh, 'B');
								$maxw=max($tw, $maxw);
								fxpdf_text($pdf, $la, 0.0, $yl, -$fh, 'left', 'bold');
							}
							$xw=$maxw+$ccw;

							$xla=array();
							$yl=$ylb;
							for($lct=0; $lct<2; $lct++)
							{
								if($lct)
								{
									$rgb1=$style['psauf'];	// Aufgabe
									$yl += ($fh*1.3);
								}
								else
									$rgb1=$style['pspro'];	// Projekt

								$plx1=fxpdf_getx($xw);
								foreach($sarr as $st => $cls)
								{
									$rgb2=$style[$cls]; // Statusfarbe

									$plx2=$plx1+$ccw;
									$plxm=$plx1+($plx2-$plx1)/2.0;
									$prx1=$plx2+$ccw;
									$prx2=$prx1+$ccw;
									$prxm=$prx1+($prx2-$prx1)/2.0;
									$py2=fxpdf_gety($yl);
									$py1=$py2-$lh;
									$pym=$py2-$lh/4.0;

									if($lct)	// Aufgabe
									{
										$xla[]=$plx1;

										$pym=$py2;
										fxpdf_pst_sollbalken($pdf, true, $plx1+$sf_100, $prx2-$sf_100, $py1, $pym, -$mf);
										fxpdf_pst_sollpk($pdf, true, true, $plx1, $plxm, $plx2, $py1, $pym, $py2, $rgb2, -$mf, $st);
										fxpdf_pst_sollpk($pdf, true, false, $prx1, $prxm, $prx2, $py1, $pym, $py2, $rgb2, -$mf, $st);

										$txt=' ('.meldung($st).')';
										$tw=fxpdf_strlen($pdf, $txt, -$lh);
										fxpdf_text($pdf, $txt, '&'.$prx2, '&'.($py2-1.5*$mf), -$lh, 'left', 'normal');
										$plx1=$prx2+$tw+$ccw;
									}
									else if($st == 297)		// Projekte
									{
										fxpdf_pst_sollbalken($pdf, false, $plx1+$sf_100, $prx2-$sf_100, $py1, $pym, -$mf);
										fxpdf_pst_sollpk($pdf, false, true, $plx1, $plxm, $plx2, $py1, $pym, $py2, $rgb2, -$mf, $st);
										fxpdf_pst_sollpk($pdf, false, false, $prx1, $prxm, $prx2, $py1, $pym, $py2, $rgb2, -$mf, $st);
									}
								}
							}

							// Meilenstein
							if($larr[2])
							{
								$dfla=explode('|',meldung(3205));	// dynamisch|fest|dyn.|fest|Dynamisch|Fest|Dyn.|Fest|D|F|Start|Anfang|Ende|S|A|E

								$yl += ($fh*1.3);

								$uy=fxpdf_gety($yl);
								$sy=$uy-$lh;
								for($l=0; $l<2; $l++)
								{
									$ul=$xla[$l];
									$ur=$ul+$lh;
									$sx=$ul+($ur-$ul)/2.0;
									fxpdf_pst_mst_draw($pdf, $sx, $ul, $ur, $sy, $uy, -$mf, ($l+1)%2);

									fxpdf_text($pdf, ' ('.$dfla[$l].')', '&'.$ur, '&'.($uy-1.5*$mf), -$lh, 'left', 'normal');
								}
							}

							// Abhängigkeiten
							if($larr[3])
							{
								$yl += ($fh*1.3);
								$abxl=fxpdf_getx($xw);
								$abxd=$abxl+$ccw;
								$abyd=fxpdf_gety($yl-$fh*0.2);
								fxpdf_line($pdf, '&'.$abxl, '&'.$abyd, '&'.$abxd, '&'.$abyd, 0.0, 0.0, -$sf_025, '888888');
								fxpdf_pst_arrow($pdf, $abxd+$mf, $abyd, $ccw, -$mf);
							}
						}
					}
				}
				else if(($ci['tr'] == 92) && !$lc)		// Sonderfall: Urlaubs-/Stundenplan
				{
					$lw=$cw+64*$mf;
					if($pdf_array['leg'] == '1')
					{
						$yc=$fh*9.0;
						if(sizeof($starr)*$lw > $GLOBALS['pdf_array']['w_body']-5.0)
							$yc += $fh;
					}
					else
						$yc=$fh*5.0;
					if(!$cc)
					{
						$txt=fxpdf_strlen($pdf, meldung(364,false), -$tfs, 'B', $pdf_array['w_body']);	// Urlaubs-/Stundenplan
						if(strlen($txt))
						{
							$title=$txt;
							fxpdf_text($pdf, $txt, 0.0, $tfs, -$tfs, 'left', 'bold');
						}

						// LEGENDE
						if($pdf_array['leg'] == '1')
						{
							$yl=$fh*5.0;
							$txt=meldung(1994);	// Legende:
							$tw=fxpdf_strlen($pdf, $txt, -$lfs, 'B');
							fxpdf_text($pdf, $txt, 0.0, $yl, -$lfs, 'left', 'bold');
							$yl += 1.0*$mf;
							fxpdf_line($pdf, 0.0, $yl, $tw, $yl, 0.0, 0.0, -$sf_050, '000000');

							$lh=$fh*0.75;

							$yl += $fh*1.4;
							$px1=0.00;
							foreach($starr as $tid => $antrag)
							{
								$px2=$px1+$cw;
								if($px1+$lw > $GLOBALS['pdf_array']['w_body']-5.0)
								{
									$px1=0.0;
									$px2=$cw;
									$yl += $fh;
								}

								fxpdf_pst_bg($pdf, $px1, $yl-$lh, $px2, $yl, 0, 0, 'au'.$tid);

								$txt=meldung($sttarr[$tid],false);
								fxpdf_text($pdf, $txt, $px2+$cw/2, $yl-$mf, -$lh, 'left', 'normal');
								$px1=$px2+64*$mf;
							}
						}
					}
				}
				else if(($ci['tr'] == 264) && !$lc)		// Sonderfall: Leistungsarten-Stundenreport
				{
					$yc=$fh*5.0;
					if(!$cc)
					{
						$txt=fxpdf_strlen($pdf, meldung(3199,false), -$tfs, 'B', $pdf_array['w_body']);	// Leistungsarten-Stundenreport
						if(strlen($txt))
						{
							$title=$txt;
							fxpdf_text($pdf, $txt, 0.0, $tfs, -$tfs, 'left', 'bold');
						}
					}
				}
				else if((($ci['tr'] == 14) || ($ci['tr'] == 43)) && !$lc)		// Sonderfall: Ressourcenplan/Kontrollzentrum
				{
					$lw=$cw+64*$mf;
					if($pdf_array['leg'] == '1')
					{
						$yc=$fh*10.0;
						if(sizeof($starr)*$lw > $GLOBALS['pdf_array']['w_body']-5.0)
							$yc += $fh;
					}
					else
						$yc=$fh*5.0;
					if(!$cc)
					{
						if($ci['tr'] == 14)
							$txt=fxpdf_strlen($pdf, meldung(1143,false), -$tfs, 'B', $pdf_array['w_body']);	// Übersicht der aktuellen Projekte
						else
							$txt=fxpdf_strlen($pdf, meldung(2129,false), -$tfs, 'B', $pdf_array['w_body']);	// Verfügbarkeitsplan
						if(strlen($txt))
						{
							$title=$txt;
							fxpdf_text($pdf, $txt, 0.0, $tfs, -$tfs, 'left', 'bold');
						}

						// LEGENDE
						if($pdf_array['leg'] == '1')
						{
							$yl=$fh*5.0;
							$txt=meldung(1994);	// Legende:
							$tw=fxpdf_strlen($pdf, $txt, -$lfs, 'B');
							fxpdf_text($pdf, $txt, 0.0, $yl, -$lfs, 'left', 'bold');
							$yl += 1.0*$mf;
							fxpdf_line($pdf, 0.0, $yl, $tw, $yl, 0.0, 0.0, -$sf_050, '000000');

							$lh=$fh*0.75;

							// 770:Verfügbar, 1132:Nicht verfügbar, 2465:Frei, 1070:Überlastet, 772:Projekt
							$sparr=array(770=>'au000', 1132=>'au100', 2465=>'au_ud', 1070=>'au200', 772=>'pr100');
							$yl += $fh*1.4;
							$px1=0.00;
							foreach($sparr as $tid => $au)
							{
								$px2=$px1+$cw;
								fxpdf_pst_bg($pdf, $px1, $yl-$lh, $px2, $yl, 0, 0, $au);

								$txt=meldung($tid,false);
								fxpdf_text($pdf, $txt, $px2+$cw/2, $yl-$mf, -$lh, 'left', 'normal');
								$px1=$px2+64*$mf;
							}

							$yl += $fh*1.4;
							$px1=0.00;
							foreach($starr as $tid => $antrag)
							{
								$px2=$px1+$cw;
								if($px1+$lw > $GLOBALS['pdf_array']['w_body']-5.0)
								{
									$px1=0.0;
									$px2=$cw;
									$yl += $fh;
								}

								fxpdf_pst_bg($pdf, $px1, $yl-$lh, $px2, $yl, 0, 0, 'au'.$tid);

								$txt=meldung($sttarr[$tid],false);
								fxpdf_text($pdf, $txt, $px2+$cw/2, $yl-$mf, -$lh, 'left', 'normal');
								$px1=$px2+64*$mf;
							}
						}
					}
				}
				else if(!$lc && fxIsArray($_GET) && isset($_GET['headline']) && strlen($_GET['headline']))
				{
					$yc=$fh*5.0;
					if(!$cc)
					{
						$txt=fxpdf_strlen($pdf, $_GET['headline'], -$tfs, 'B', $pdf_array['w_body']);	// Headline?
						if(strlen($txt))
						{
							$title=$txt;
							fxpdf_text($pdf, $txt, 0.0, $tfs, -$tfs, 'left', 'bold');
						}
					}
				}
				if($tyc0 < 0.0)
					$tyc0=$yc;
				$yc=0.0;
				if(!$lc)
					$yc=$tyc0;
				$syc=$yc;

				// Zeile 0-2
				$nlc=false;
				if(!$lc)
				{
					$nlc=true;
					$lcol=0;
					for($nd=0; $nd<3; $nd++)
					{
						$xc=0.0;
						if(!$cut)
							$xc=dpdf_leftside($pdf, $nd, $xc, $yc);
						$xc=dpdf_rightside($pdf, $nd, $xc, $yc);
						$yc += $pdf_array['cheight_t'];
					}
				}
				if($tyc1 < 0.0)
					$tyc1=$yc;
//echo('-> $seite="<B>'.$seite.'</B>", $lcol="<B>'.$lcol.'</B>", $rcol="<B>'.$ci['rcol'].'</B>"<br />');
//fxDebug($pscal[2][$lcol+$ci['lcol']], 'Letzte Spalte');

				// Zeile 3-?
				$yc=0.0;
				if(!$lc)
					$yc=$tyc1;
				$ycs=$yc;
//fxDebug($ycs,'$ycs: $tyc0='.$tyc0.', $tyc1='.$tyc1.' -- $yc='.$yc.', $syc='.$syc.' -- $lc='.$lc.', $seite='.$seite.' -- $lcol='.$lcol.', $rcol='.$ci['rcol']);
				$xcs=0.0;
				$ylz=0.0;

				$msr=array();
				$dpr=array();

				while(($lc < $ci['lines']) && ($yc+$pdf_array['cheight'] < $pdf_array['h_body']))
				{
					$disp=true;
					if(isset($ci[3+$lc]['disp']) && !$ci[3+$lc]['disp'])
						$disp=false;

					if($disp)
					{
						if($ci[3+$lc][0]['text'] == '^')	// Leerzeile, Teil 1
						{
							if($yc)
							{
								$xce=$xcs + ($lcol+1-$cc)*$pdf_array['cwidth'];
								fxpdf_box($pdf, 0.0, $yc-(0.25*$pdf_array['fac']), $xce, $yc, '000000', '0000');
								$ylz=$yc;
							}
						}
						else
						{
							$xc=0.0;
							if(!$cut)
							{
								$xc=dpdf_leftside($pdf, 3+$lc, $xc, $yc);
								$xcs=$xc;
							}
							$xcm=$xc;
							$xc=dpdf_rightside($pdf, 3+$lc, $xc, $yc);
							if($GLOBALS['pscal'][3+$lc][0]['dep'] && !isset($dpr[$lc]))
							{
								$dpr[$lc]=true;
								dpdf_rightside($pdf, 3+$lc, $xcm, $yc, 'dep');
							}
							if($GLOBALS['pscal'][3+$lc][0]['mst'] && !isset($msr[$lc]))
							{
								$msr[$lc]=true;
								dpdf_rightside($pdf, 3+$lc, $xcm, $yc, 'mst');
							}
							$yc += $pdf_array['cheight'];
							if($ylz)	// Leerzeile, Teil 2
							{
								fxpdf_box($pdf, 0.0, $ylz, $xce, $ylz+(0.25*$pdf_array['fac']), '000000', '0000');
								$ylz=0.0;
							}
						}
					}
					$lc++;
				}

				// Folgen noch Meilensteine?
				$mlc=$lc;
				while(($mlc < $ci['lines']) && fxIsArray($GLOBALS['pscal'][3+$mlc][0]['mst']) && !isset($msr[$mlc]))
				{
					$disp=true;
					if(isset($ci[3+$mlc]['disp']) && !$ci[3+$mlc]['disp'])
						$disp=false;

					if($disp)
					{
						$msr[$mlc]=true;
						dpdf_rightside($pdf, 3+$mlc, $xcm, $yc, 'mst');
					}

					$mlc++;
				}

				// Raster
				$lw1=0.05*$pdf_array['fac'];
				$lw2=0.20*$pdf_array['fac'];

				$ycsr=$ycs;
				$ycer=$yc;

				$xce=$xcs + ($lcol+1-$cc)*$pdf_array['cwidth'];

//echo("lcol=$lcol, cc=$cc -> xcs=$xcs, xce=$xce -- ycs=$ycs, yce=$yce -- ycsr=$ycsr, ycer=$ycer<br />");
//fxpdf_box($pdf, $xcs, $ycsr, $xce, $ycer, 'ffff00');

				// ...Horizontal
				$ycsb=$ycsr;
				$ycsr += $pdf_array['cheight'];
				while($ycsr < $ycer)
				{
//echo('<div style="margin-top:8px;padding:8px;border:1px solid #000000;background:#ccccff;"><b>Ha-Line</b> '.$xcs.', '.$ycsr.', '.$xce.', '.$ycsr.'</div>'.$GLOBALS['nl']);
					fxpdf_line($pdf, $xcs, $ycsr, $xce, $ycsr, 0.0, 0.0, $lw1, 'aaaaaa');
					$ycsr += $pdf_array['cheight'];
				}

				// ...Vertikal
				$rac=$acc;
				$xcsb=$xcs;
				$xcs += $pdf_array['cwidth'];
				while(($xcs < $xce) && ($xcs < $pdf_array['w_body']))
				{
					$vlw=$lw1;
					$vyt=$ycsb;
					$vyb=$ycer;
					$vlc='aaaaaa';
					if($pscal[0][$rac+1]['text'])
					{
						$vlw=$lw2;
						$vyt -= ($pdf_array['cheight_t']*3);
						$vlc='222222';
					}
					else if($pscal[1][$rac+1]['text'])
					{
						$vlw=$lw2;
						$vyt -= ($pdf_array['cheight_t']*2);
						$vlc='666666';
					}
					$vyt=max($vyt,0.0);
					fxpdf_line($pdf, $xcs, $vyt, $xcs, $vyb, 0.0, 0.0, $vlw, $vlc);
//echo('<div style="margin-top:1px;padding:4px;border:1px solid #000000;background:#ffffff;color:#'.$vlc.';"><b>V-Line</b> ('.$xcs.', '.$vyt.', '.$xcs.', '.$vyb.', #'.$vlc.')</div>'.$GLOBALS['nl']);
					$xcs += $pdf_array['cwidth'];
					$rac++;
				}

				// Überflüssiges abschneiden
				$cutcol='ffffff';
				$xcm=min($xce, $pdf_array['w_body']);
				// ...Links
				if($cut)
				{
					$xcsl=0.0;
					fxpdf_box($pdf, '&0.0', '&0.0', 0.0, '&'.$pdf_array['height'], $cutcol, '0000');
					fxpdf_line($pdf, 0.0, $syc, 0.0, $ycer, 2.0*$pdf_array['fac'], 0.5*$pdf_array['fac'], 0.15*$pdf_array['fac']);
				}
				else
				{
					$xcsl=2.25*$pdf_array['fac'];
					fxpdf_line($pdf, $xcsb, $ycsb-($pdf_array['cheight_t']*3), $xcsb, $ycer, 0.0, 0.0, $lw2, '222222');
				}
				// ...Oben
				if($seite > 1)	// Oben
				{
					$ycdo=$syc;
					fxpdf_box($pdf, '&0.0', '&0.0', '&'.$pdf_array['width'], $ycsb, $cutcol, '0000');
					fxpdf_line($pdf, 0.0, $ycsb, $xcm, $ycsb, 2.0*$pdf_array['fac'], 0.5*$pdf_array['fac'], 0.15*$pdf_array['fac']);
				}
				else
				{
					$ycdo=$syc+(2.25*$pdf_array['fac']);
//echo('<div style="margin-top:8px;padding:8px;border:1px solid #000000;background:#ccccff;"><b>H2-Line</b> 0.0, '.$ycsb.', '.$xce.', '.$ycsb.'</div>'.$GLOBALS['nl']);
					fxpdf_line($pdf, 0.0, $ycsb, $xce, $ycsb, 0.0, 0.0, $lw2, '222222');
				}
				// ...Rechts
				if(($lcol < ($ci['rcol']-1)) || ($xce > $pdf_array['w_body']))
				{
					$xcsr=0.0;
					fxpdf_box($pdf, $xcm, '&0.0', '&'.$pdf_array['width'], '&'.$pdf_array['height'], $cutcol, '0000');
					fxpdf_line($pdf, $xcm, $syc, $xcm, $ycer, 2.0*$pdf_array['fac'], 0.5*$pdf_array['fac'], 0.15*$pdf_array['fac']);
				}
				else
				{
					$xcsr=2.25*$pdf_array['fac'];
					fxpdf_line($pdf, $xcm, $ycsb, $xcm, $ycer);
				}
				// ...Unten
				fxpdf_box($pdf, '&0.0', $ycer, '&'.$pdf_array['width'], '&'.$pdf_array['height'], $cutcol, '0000');
				if($lc < $ci['lines'])
				{
					$ycdu=0.0;
					fxpdf_line($pdf, 0.0, $ycer, $xcm, $ycer, 2.0*$pdf_array['fac'], 0.5*$pdf_array['fac'], 0.15*$pdf_array['fac']);
				}
				else
				{
					$ycdu=0.25*$pdf_array['fac'];
					fxpdf_line($pdf, $xcsb, $ycer, $xcm, $ycer);
				}

				// Schatten zeichen
				if($ci['tr'] != 26)
				{
					if($lcol >= ($ci['rcol']-1))	// Rechts
						fxpdf_box($pdf, $xcm+(0.25*$pdf_array['fac']), $ycdo, $xcm+(2.25*$pdf_array['fac']), $ycer+$ycdu, 'eeeeee', '0000');
					if($lc >= $ci['lines'])			// Unten
						fxpdf_box($pdf, $xcsl, $ycer+(0.25*$pdf_array['fac']), $xcm+$xcsr, $ycer+(2.25*$pdf_array['fac']), 'eeeeee', '0000');
				}

				// Testversion
				if($testversion)
					fxpdf_diagonal($pdf, true);

				// Aktuelle Seite abschliessen, so dass keine Aenderungen mehr vorgenommen werden können.
				// Vorher noch die FeRox-Fusszeile setzen
				if(!isset($pdf_array['f_nofoot']))
					fxpdf_clipping($pdf, 2, $seite.chr(65+$cut), true, $dmb);
				fxpdf_page($pdf, 'end');

				$seite++;
			}

			$cc=$lcol;
//echo('<div style="margin-top:8px;padding:8px;border:1px solid #000000;background:#ff8888;">$cc=$lcol='.$cc.' -- (($xce='.$xce.' > $pdf_array[w_body]='.$pdf_array['w_body'].') && ($cc='.$cc.' >= $max_cc='.$max_cc.'))</div>'.$GLOBALS['nl']);
			if(($xce > $pdf_array['w_body']) && ($cc >=  $max_cc))
			{
				$cc=$max_cc-1;
//echo('<div style="margin-top:8px;padding:8px;border:1px solid #000000;background:#ff8888;">$cc='.$cc.' -- ($max_cc='.$max_cc.' - 1)</div>'.$GLOBALS['nl']);
			}

			$cut++;

			if($cc >= $max_cc)
				break;

			$emc++;
			if($emc > $eme)
				break;
		}

		// Statusbericht
		$show_status=true;
		if($show_status && ($ci['tr'] == 26) && ($pdf_array['att'] == '1'))
		{
			$strans_html=array('&nbsp;'=>' ', '&quot;'=>'"');
			$lc=3;
			$lcm=$ci['lines']+3;
			$ntt=array();
			$ttc=0;
			while($lc < $lcm)	// Zeilen durchgehen
			{
				$disp=true;
				if(isset($ci[$lc]['disp']) && !$ci[$lc]['disp'])
					$disp=false;

				if($disp && ($pscal[$lc][0]['pid'] > 0))
				{
					$ntt[$ttc]=array('head'=>$ci[$lc][0]['text']);
					$sc=(int)$pscal[$lc][0]['sk'];
					if(!$sc)
						$sc=4;
					if(substr($pscal[$lc][0]['pt'],0,1) == 'p')	// Projekt
						$ntt[$ttc]['mode']='bold';
					else
						$ntt[$ttc]['mode']='normal';

					$tooltip=$pscal[$lc][$sc]['tooltip_js'];
//echo('<pre style="border:1px dotted black;background:#ffffff;margin-top:2px;padding:4px;width:1400px;white-space:normal;">'.fxHtmlEncode($tooltip).'</pre>');
					$cp=0;
					$kp=0;
					$ntex=array();
					$trsp=stripos($tooltip, '<tr>', $cp);
					while($trsp !== false)
					{
						$cp=$trsp+4;
						$trep=stripos($tooltip, '</tr>', $cp);
						if($trep !== false)
						{
							$trs=substr($tooltip, $trsp+4, $trep-$trsp-4);
							$cp =$trep+5;
						}
						else
						{
							$trs=substr($tooltip, $trsp+4);
							$cp =strlen($tooltip);
						}

						if(strlen($trs))
						{
							$lit='';
							$val='';
							$cat='';
							$fp=0;
							$tdsp=stripos($trs, '<td', $fp);
							while($tdsp !== false)
							{
								$tdsa=strpos($trs, '>', $tdsp);
								$fp=$tdsa+1;
								$tdep=stripos($trs, '</td>', $fp);
								$tdo=substr($trs, $fp, $tdep-$fp);
								$tds=strip_tags(strtr($tdo, array('<br>'=>'*br*', '<br />'=>'*br*')));

								if(strlen($tds))
								{
//echo('<pre style="border:1px dotted black;background:#ffffff;margin-top:2px;padding:4px;width:1400px;white-space:normal;">'.fxHtmlEncode($tds).'</pre>');
									// Projektkategorie?
									if(substr($tdo,0,5) == '<span')
									{
										$bgp=stripos($tdo,'background:#');
										if($bgp !== false)
											$cat=substr($tdo,$bgp+12,6);
									}
									if(substr($tds,-1) == ':')	// Literal
										$lit=substr($tds,0,-1);
									else
										$val=$tds;
								}
								$tdsp=stripos($trs, '<td nowrap', $tdep+1);
							}

							if(strlen($val))
							{
								if(strlen($lit))
									$ntex[$kp]['lit']=fxpdf_convert(strtr($lit, $strans_html)).':';
								$ntex[$kp]['val']=fxpdf_convert(strtr($val, $strans_html));
								if(strlen($cat))
									$ntex[$kp]['cat']=$cat;
								$kp++;
							}
						}
						$trsp=stripos($tooltip, '<tr>', $cp);
					}
					if(fxIsArray($ntex))
						$ntt[$ttc]['tip']=$ntex;
					$ttc++;
				}
				$lc++;
			}
			fxpdf_anhang($pdf, $ntt);
		}

		// Aktuelles PDF-Dokument schliessen
		$pdf_data=fxpdf_page($pdf, 'close');
		$pdf_len=strlen($pdf_data);
	}

	// Spreadsheet
	if(strlen($GLOBALS['_ext_spreadsheet']) && !isset($_preview_config))
	{
/*
//fxDebug($ci, '$ci');
		// Währung und andere Einheiten
		$units=getUnits();

		// CSV
		$GLOBALS['csv_array']=array('file'=>$GLOBALS['tmppath'].'~'.$ad.'.csv', 'lcol'=>0, 'headers'=>array(), 'units'=>array(), 'lines'=>array());

//fxDebug($GLOBALS['fxptdata']['cal'], 'cal');
		if(!class_exists('PHPExcel'))
			require('class_phpexcel.inc');

		// Excel: Create new PHPExcel object
		$pe=new PHPExcel();

		// Excel... set document properties
		if(strlen($GLOBALS['pdf_array']['f_text']))
			$pe->getProperties()->setCreator(fxpExcelUTF8($GLOBALS['pdf_array']['f_text']));
		else
			$pe->getProperties()->setCreator('fx-project');
		$pe->getProperties()->setLastModifiedBy(fxpExcelUTF8($GLOBALS['fxpglobals']['uname']));
		if(strlen($title))
		{
			$pe->getProperties()->setTitle(fxpExcelUTF8($title));
			$pe->getProperties()->setSubject(fxpExcelUTF8($title));
		}
//			$pe->getProperties()->setDescription('Test');
//			$pe->getProperties()->setKeywords('office 2007 openxml php');
//			$pe->getProperties()->setCategory('Test result file');

		// Excel... set default font
		$pe->getDefaultStyle()->getFont()->setName('Arial')->setSize(12);

		// Excel... define default borders
		$cellborder=array('borders'=>array('outline'=>array('style'=>PHPExcel_Style_Border::BORDER_THIN, 'color'=>array('argb'=>'FFAAAAAA'))));
		$cellborderp=array('borders'=>array('left'=>array('style'=>PHPExcel_Style_Border::BORDER_THICK, 'color'=>array('argb'=>'FF000000')), 'right'=>array('style'=>PHPExcel_Style_Border::BORDER_THICK, 'color'=>array('argb'=>'FF000000')), 'top'=>array('style'=>PHPExcel_Style_Border::BORDER_THIN, 'color'=>array('argb'=>'FF000000'))));
		$cellbordert=array('borders'=>array('top'=>array('style'=>PHPExcel_Style_Border::BORDER_THIN, 'color'=>array('argb'=>'FF777777')), 'bottom'=>array('style'=>PHPExcel_Style_Border::BORDER_THIN, 'color'=>array('argb'=>'FF777777')), 'left'=>array('style'=>PHPExcel_Style_Border::BORDER_THICK, 'color'=>array('argb'=>'FF555555')), 'right'=>array('style'=>PHPExcel_Style_Border::BORDER_THICK, 'color'=>array('argb'=>'FF555555'))));
		$frameborder=array('borders'=>array('outline'=>array('style'=>PHPExcel_Style_Border::BORDER_THIN, 'color'=>array('argb'=>'FF000000'))));
		$leftborder=array('borders'=>array('left'=>array('style'=>PHPExcel_Style_Border::BORDER_THIN, 'color'=>array('argb'=>'FF555555'))));
		$lineborder=array('borders'=>array('bottom'=>array('style'=>PHPExcel_Style_Border::BORDER_THIN, 'color'=>array('argb'=>'FF000000'))));

		// Excel... add cell data
		$mca=array();
		$bca=array();
		$lcell=1;
		$lline=1;
		$fixcols=0;
		if(isset($GLOBALS['fxptdata']['cal'][0][0]['fixcols']))
			$fixcols=1+(int)$GLOBALS['fxptdata']['cal'][0][0]['fixcols'];
		foreach($GLOBALS['fxptdata']['cal'] as $cln => $cca)
		{
			if(fxIsArray($cca))
			{
				$lline=max($lline,1+$cln);
				foreach($cca as $ccl => $cla)
				{
					// Fixcols
					if(!$fixcols && ($cln == 2) && isset($cla['text']) && (substr($cla['text'],0,4) === '<div'))
						$fixcols=$ccl+1;

					// Cell
					$cell=fxpExcelCell($ccl+1, $cln+1);

					// Border 0+1
					if(($cln < 2) && ($ccl >= $ci['lcol']))
					{
						$bc=fxpExcelCell($ccl+1);
						if(!isset($bca[$bc]))
							$bca[$bc]=$cln+1;
					}

					// Merge
					$ccs=1;
					$crs=1;
					if(isset($cla['colspan']))
						$ccs=max(1, (int)$cla['colspan']);
					if(isset($cla['rowspan']))
						$crs=max(1, (int)$cla['rowspan']);
					if(($ccs > 1) || ($crs > 1))
						$mca[$cell]=array('dcell'=>fxpExcelCell($ccl+$ccs, $cln+$crs), 'bold'=>false, 'align'=>"l", 'color'=>"", 'prt'=>"");
					$lcell=max($lcell, $ccl+$ccs);

					// Style variables
					$lb=false;
					$bold=false;
					$align='';
					$color='';
					$ue=-1;
					$fdf=0;
					$fnf=0;
					$ftf=0;
					$fpf=0;
					$faf=0;
					$cfm='';

					// ...Color?
					$rot=0;
					$prt='';
					$bgc='';
					$filltype=PHPExcel_Style_Fill::FILL_SOLID;
					if(!strlen($color) && isset($cla['color']))
						$color=trim($cla['color']);
					if(!strlen($color) && isset($cla['class']) && isset($GLOBALS['style'][$cla['class']]))
						$color=trim($GLOBALS['style'][$cla['class']]);
					if(!strlen($color))
					{
						if(isset($cla['pkl']))
						{
							$prt=strtolower(substr($GLOBALS['fxptdata']['cal'][$cln][0]['pt'],0,1));
							$filltype=PHPExcel_Style_Fill::FILL_GRADIENT_LINEAR;
							$rot=90;
							if($prt == 't')	// Task
								$color='ffffffbbbbbb';
							else			// Project
								$color='ffffff888888';
						}
						else if(isset($cla['background']) && strlen($cla['background']))
						{
							$bgc=trim($cla['background']);
							$bga=fxpdf_pst_bgcol($bgc);
							$color=$bga['rgb1'];
							if(strlen($bga['rgb2']))
							{
								$color .= $bga['rgb2'];
								$filltype=PHPExcel_Style_Fill::FILL_PATTERN_DARKHORIZONTAL;
							}
							else if(strlen($bga['lrgb1']))
							{
								$color .= $bga['lrgb1'];
								$filltype=PHPExcel_Style_Fill::FILL_PATTERN_GRAY0625;
							}
							else if(strlen($bga['lrgb2']))
							{
								$color=$bga['lrgb2'].$color;
								$filltype=PHPExcel_Style_Fill::FILL_PATTERN_GRAY125;
							}
						}
						else if(isset($cla['typ']) && ($cla['typ'] == 'nv'))
							$color='ffffff';
						else
							$color='eeeeee';
					}
					if(isset($mca[$cell]))
					{
						$mca[$cell]['color']=$color;
						$mca[$cell]['prt']=$prt;
					}

					// Text
					$text=trim($cla['text']);
					if((substr($text,0,1) == '~') || (substr($text,0,1) == '^') || (substr($text,0,1) == '|'))
						$text=trim(substr($text,1));
					if(!strlen($text) && isset($cla['itext']))
						$text=trim($cla['itext']);
					if(!strlen($text) && strlen($bgc) && (substr($bgc,0,2) == 'au') || (substr($bgc,0,2) == 'ap'))
					{
						if(isset($cla['tooltip']) && (substr($cla['tooltip'],-1) == '%'))
							$text=trim($cla['tooltip']);
						else if(isset($cla['tooltip_js']) && (substr($cla['tooltip_js'],-1) == '%'))
							$text=trim($cla['tooltip_js']);
					}
					if(strlen($text))
					{
						// ...Line break?
						if(stripos($text,'<br />') !== false)
						{
							$text=str_ireplace('<br />', "\n", $text);
							$lb=true;
						}
						// ...Bold?
						if((stripos($text,'<b>') !== false) || (stripos($text,'</b>') !== false))
						{
							$bold=true;
							if(isset($mca[$cell]))
								$mca[$cell]['bold']=$bold;
						}
						// ...Align?
						$align='l';
						if(isset($cla['align']))
						{
							$align=strtolower(substr($cla['align'],0,1));
							if(isset($mca[$cell]))
								$mca[$cell]['align']=$align;
						}

						$text=trim(strip_tags(strtr($text, $trans_html)));
						// ...Unit entry?
						$uc='';
						foreach($units as $up => $unit)
						{
							$chk=' '.$unit;
							if(substr($text, -strlen($chk)) == $chk)
							{
								$text=trim(substr($text,0,-strlen($chk)));
								$ue=$up;
								$uc=$unit;
								break;
							}
						}
						$text=fxpExcelUTF8($text);
						// ...Format?
						if(strlen($text))
						{
							for($t=0; $t<strlen($text); $t++)
							{
								$cc=substr($text,$t,1);
								if(($cc == ',') || ($cc == '.') || ($cc == '-'))			// ,.-
								{
									$fdf++;
									if(($fdf == 2) && (substr($text,$t-4,1) != $cc))
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
						}
						if($ue >= 0)
						{
							$text=get_double($text);
							$cfm='###,###,##0.00" '.fxpExcelUTF8($units[$ue]).'"';
						}
						else if($ftf || $faf || (!$fdf && !$fnf))
							$text=(string)$text;
						else
						{
							$cfm='###,###,##0';
							if($fdf)
							{
								$text=get_double($text);
								$cfm .= '.00';
							}
							else
								$text=(int)$text;
							if($fpf)
							{
								$cfm .= '"%"';
								$uc='%';
							}
						}
					}

					// ...Write cell
					$cr=1;
					if(($cln > 2) && ($ccl >= $ci['lcol']) && (!strlen($text) || ($cla['typ'] == 'nv')))
					{
						$cr=$ccs;
						if(isset($mca[$cell]))
							unset($mca[$cell]);
//echo('<div style="margin-top:2px;margin-right:2px;padding:4px;border-radius:4px;background:#eeeeee;">[<b>'.$cell.'</b>] $cr='.$cr.'</div>');
					}
					$ca=1;
					while($ca <= $cr)
					{
						// EXCEL
						$wcell=fxpExcelCell($ccl+$ca, $cln+1);

						if(strlen($text))
						{
							$pe->setActiveSheetIndex(0)->setCellValue($wcell, $text);
							$pe->getActiveSheet(0)->getStyle($wcell)->getAlignment()->setVertical(PHPExcel_Style_Alignment::VERTICAL_CENTER);
						}
						if(strlen($cfm))
							$pe->getActiveSheet(0)->getStyle($wcell)->getNumberFormat()->setFormatCode($cfm.';[Red]-'.$cfm);
						if($align == 'c')
							$pe->getActiveSheet(0)->getStyle($wcell)->getAlignment()->setHorizontal(PHPExcel_Style_Alignment::HORIZONTAL_CENTER);
						else if($align == 'r')
							$pe->getActiveSheet(0)->getStyle($wcell)->getAlignment()->setHorizontal(PHPExcel_Style_Alignment::HORIZONTAL_RIGHT);
						else if($align == 'l')
							$pe->getActiveSheet(0)->getStyle($wcell)->getAlignment()->setHorizontal(PHPExcel_Style_Alignment::HORIZONTAL_LEFT);
						if($lb)
							$pe->getActiveSheet(0)->getStyle($wcell)->getAlignment()->setWrapText(true);
						if($bold)
							$pe->getActiveSheet(0)->getStyle($wcell)->getFont()->setBold(true);
						if(strlen($color) == 12)
						{
							$pe->getActiveSheet(0)->getStyle($wcell)->getFill()->setFillType($filltype)->getStartColor()->setARGB('FF'.strtoupper(substr($color,0,6)));
							$pe->getActiveSheet(0)->getStyle($wcell)->getFill()->setFillType($filltype)->getEndColor()->setARGB('FF'.strtoupper(substr($color,6,6)));
						}
						else if(strlen($color))
							$pe->getActiveSheet(0)->getStyle($wcell)->getFill()->setFillType($filltype)->getStartColor()->setARGB('FF'.strtoupper($color));
						if($rot)
							$pe->getActiveSheet(0)->getStyle($wcell)->getFill()->setRotation($rot);
						if(!isset($mca[$wcell]))
							$pe->getActiveSheet(0)->getStyle($wcell)->applyFromArray(${'cellborder'.$prt});

//echo('<div style="margin-top:2px;margin-right:2px;padding:4px;border-radius:4px;background:#ffffff;">');
//fxDebug($cla,$wcell.' ('.$cell.'): $ccs='.$ccs.', $crs='.$crs.' -- $ue='.$ue.', type='.gettype($text).' -- $bold='.$bold.', $align='.$align.' -- $color='.$color);
//echo('<font class="blue">'.fxHtmlEncode($text).'</font></div>');

						// CSV
						$cccl=$ccl+$ca-1;
						if($cln == 2)	// Headers
						{
							if($ca == 1)
								$GLOBALS['csv_array']['headers'][$cccl]=$text;
							else
								$GLOBALS['csv_array']['headers'][$cccl]='';
						}
						else if($cln > 2)
						{
							$GLOBALS['csv_array']['lcol']=max($GLOBALS['csv_array']['lcol'],$cccl);
							$ccln=$cln-3;
							if(strlen($uc) && !isset($GLOBALS['csv_array']['units'][$cccl]))
								$GLOBALS['csv_array']['units'][$cccl]=$uc;
							if(!isset($GLOBALS['csv_array']['lines'][$ccln]))
								$GLOBALS['csv_array']['lines'][$ccln]=array();
							$GLOBALS['csv_array']['lines'][$ccln][$cccl]=$text;
						}

						$ca++;
					}
				}
			}
		}

		// Excel... merge cells
//fxDebug($mca,'$mca');
		if(sizeof($mca))
		{
			foreach($mca as $scell => $dca)
			{
				$mcell=$scell.':'.$dca['dcell'];
				$pe->setActiveSheetIndex(0)->mergeCells($mcell);
				$pe->getActiveSheet(0)->getStyle($mcell)->applyFromArray(${'cellborder'.$dca['prt']});
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
//fxDebug($bca,'$bca');
		if(sizeof($bca))
		{
			foreach($bca as $bc => $bl)
				$pe->getActiveSheet(0)->getStyle($bc.$bl.':'.$bc.$lline)->applyFromArray($leftborder);
		}
		$pe->getActiveSheet(0)->getStyle(fxpExcelCell($ci['lcol']+1,1).':'.fxpExcelCell($ci['lcol']+1,$lline))->applyFromArray($leftborder);
		$pe->getActiveSheet(0)->getStyle(fxpExcelCell(1,3).':'.fxpExcelCell($lcell,3))->applyFromArray($lineborder);
		$pe->getActiveSheet(0)->getStyle(fxpExcelCell(1,1).':'.fxpExcelCell($lcell,$lline))->applyFromArray($frameborder);

		// Excel... set column width
		$wca=array();
		$rcw=0.0;
		for($c=1; $c<=$lcell; $c++)
		{
			$cc=fxpExcelCell($c);
			$pe->getActiveSheet(0)->getColumnDimension($cc)->setAutoSize(true);
		}
		$pe->getActiveSheet()->calculateColumnWidths();
		for($c=1; $c<=$lcell; $c++)
		{
			$cc=fxpExcelCell($c);
			$cw=$pe->getActiveSheet(0)->getColumnDimension($cc)->getWidth();
			$wca[$c]=$cw;
			if($c > $ci['lcol'])
				$rcw=max($rcw,$cw);
		}
		foreach($wca as $c => $cw)
		{
			$cc=fxpExcelCell($c);
			$ncw=$cw;
			if($c > $ci['lcol'])
				$ncw=$rcw;
//echo('<div style="margin-top:2px;margin-right:2px;padding:4px;border-radius:4px;background:#eeeeee;"><b>'.$cc.'</b> width='.$ncw.' ('.$cw.')</div>');
			$pe->getActiveSheet(0)->getColumnDimension($cc)->setAutoSize(false)->setWidth($ncw);
		}

		// Excel... freeze pane
		if(!$fixcols)
			$fixcols=$ci['lcol']+1;
		$pe->getActiveSheet()->freezePane(fxpExcelCell($fixcols,4));

		// Excel... rename worksheet
		if(strlen($title))
			$pe->getActiveSheet()->setTitle(fxpExcelUTF8($title,true));
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

	// Einstellungswerte wieder zurücksetzen
	$GLOBALS['fxpglobals']['settings']=$settings_backup;

	$tr=$_backup_tr;
	$GLOBALS['fxpglobals']['tr']=$tr;
	$lang=$_backup_lang;
	$GLOBALS['fxpglobals']['lang']=$lang;

	if(isset($_preview_config))
	{
		// PDF-Dokument speichern und Redirect zum direkten Anzeigen
		$fs=$GLOBALS['tmppath'].'~'.$ad.'.pdf';
		$fe=fxSave($fs, $pdf_data);
		if(!$fe)
			echo('<head><meta http-equiv="refresh" content="0;URL=loader.php?url=config_preview.inc&pf='.$ad.'"></head>');
		die;
	}
	else
	{
		$fn=urlencode($pdf_array['filename']);
		dm_window($fn,$ad);
	}
}


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  DPDF_CAL_INFO                                                            +
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf - Mandatory parameter: ???PARAMETER???
 */
function dpdf_cal_info(&$pdf)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($pdf === false)
	{
		if(isset($GLOBALS['_preview_tr']))
			$GLOBALS['ci']['tr']=$GLOBALS['_preview_tr'];
		else
			$GLOBALS['ci']['tr']=$GLOBALS['fxpglobals']['tr'];
		if($GLOBALS['pdf_cal_info_debug'])
			echo('<B>KALENDERINFO ERMITTELN IN TRANSAKTION '.$GLOBALS['ci']['tr'].':</B><br />');	// DEL

		if(fxIsArray($GLOBALS['pscal']) && fxIsArray($GLOBALS['pscal'][2]))
		{
//fxDebug($GLOBALS['pscal'],'pscal');
			// Anzahl der Kalenderspalten ermitteln
			$lcol=0;
			$rcol=0;
			foreach($GLOBALS['pscal'][2] as $l)
			{
				if($GLOBALS['pdf_cal_info_debug'])
					fxDebug($l, "lcol=$lcol, rcol=$rcol");
				$cs=$l['colspan'];
				if(!$cs)
					$cs=1;
				if(substr($l['text'],0,4) == '<div')
					$rcol += $cs;
				else
					$lcol += $cs;
			}
			$GLOBALS['ci']['lcol']=$lcol;
			$GLOBALS['ci']['rcol']=$rcol;
			$GLOBALS['ci']['mlcol']=$lcol;
//echo('$lcol='.$lcol.', $rcol='.$rcol.'<br />');

			// Startspalte Links
			$GLOBALS['ci']['scl']=0;
			if($GLOBALS['ci']['tr'] == 26)		// Sonderfall: Projektstatus
			{
				$GLOBALS['ci']['mlcol'] -= 2;
				$GLOBALS['pscal'][0][1]['colspan']=1;
				$GLOBALS['pscal'][1][1]['colspan']=1;
			}
			else if($GLOBALS['ci']['tr'] == 92)	// Sonderfall: Urlaubs-/Stundenplan
				$GLOBALS['ci']['mlcol']--;

			if($GLOBALS['pdf_cal_info_debug'])
				fxDebug($GLOBALS['pscal'], '$mlcol='.$GLOBALS['ci']['mlcol']);

			// Anzahl der Kalenderzeilen ermitteln
			$lines=0;
			while(is_array($GLOBALS['pscal'][$lines][0]) || is_array($GLOBALS['pscal'][$lines][$lcol]))
				$lines++;
			$GLOBALS['ci']['lines']=$lines-3;

			// Sonderfall: MA-Verfügbarkeitsplan
			if(($GLOBALS['ci']['tr'] == 43) && ($GLOBALS['ci']['lines'] > 1) && isset($GLOBALS['pscal'][$lines-1][$lcol]['class']))
				$GLOBALS['ci']['lines']--;

			$GLOBALS['ci']['col']=array();

			// Max. Längen der linken Spalten ermitteln
			for($l=$GLOBALS['ci']['lines']+2; $l>2; --$l)
			{
				// Prüfen, ob Zeile angezeigt werden soll
				$disp=true;
				$pid=0;
				if(isset($GLOBALS['pscal'][$l][0]['pid']))
					$pid=$GLOBALS['pscal'][$l][0]['pid'];
				if($pid && sizeof($GLOBALS['pda']) && isset($GLOBALS['pda'][$pid]))
					$disp=$GLOBALS['pda'][$pid];
				else if(isset($GLOBALS['pscal'][$l][0]['disp']) && ($GLOBALS['pscal'][$l][0]['disp'] == 'none'))
					$disp=false;
				$GLOBALS['ci'][$l]['disp']=$disp;

				for($c=$GLOBALS['ci']['scl']; $c<$GLOBALS['ci']['mlcol']; $c++)
				{
					$colspan=max(1, (int)$GLOBALS['pscal'][$l][$c]['colspan']);
					$rowspan=max(1, (int)$GLOBALS['pscal'][$l][$c]['rowspan']);
					$lct=$GLOBALS['pscal'][$l][$c]['text'];
					$lctl=strtolower($lct);
					$entry='';
					$bold=false;
					// Prüfen, ob Input-Tag
					$itpos=strpos($lctl, '<input id=');
					if($itpos === false)
						$itpos=strpos($lctl, '<input class=');
					if($itpos !== false)
					{
						if($GLOBALS['pdf_cal_info_debug'])
							echo('<B><FONT COLOR="#00AA00">'.$l.' - '.$c.'</FONT>:<B> (INPUT) ['.fxHtmlEncode($lct).']<br />');
						$deltapos=strpos($lctl, $GLOBALS['gfxpath'].'dlt.png');
						$valpos=strpos($lctl, 'value=', $itpos);
						if($valpos !== false)
						{
							// Wert extrahieren
							$evpos=strpos($lct, '"', $valpos+7);
							if($deltapos !== false)
							{
								$endtagpos=strpos($lct, '>', $evpos)+1;
								$entry='* '.substr($lct, $valpos+7, $evpos-$valpos-7).fxpdf_convert(strtr(strip_tags(substr($lct, $endtagpos)), $GLOBALS['trans_html']));
							}
							else
								$entry=substr($lct, $valpos+7, $evpos-$valpos-7).fxpdf_convert(strtr(strip_tags($lct), $GLOBALS['trans_html']));
						}
					}
					// Prüfen, ob Checkbox
					if(!strlen($entry))
					{
						$itpos=strpos($lctl, '<input type="checkbox"');
						if($itpos !== false)
						{
							if($GLOBALS['pdf_cal_info_debug'])
								echo('<B><FONT COLOR="#0000AA">'.$l.' - '.$c.'</FONT>:<B> (CHECKBOX) ['.fxHtmlEncode($lct).']<br />');
							$ckpos=strpos($lctl, 'checked', $itpos);
							if($ckpos !== false)
								$entry='X';	// Haken bei Checkbox
							else
								$entry='-';	// Kein Haken
						}
					}
					// Prüfen, ob Checkbox-Grafiken
					if(!strlen($entry))
					{
						$itpos=strpos($lctl, 'chk0.png');
						if($itpos !== false)
							$entry='-';	// Kein Haken
					}
					if(!strlen($entry))
					{
						$itpos=strpos($lctl, 'chk1.png');
						if($itpos !== false)
							$entry='X';	// Haken
					}
					// Prüfen, ob Projekt oder Aufgabe
					$psp=0;
					if(!strlen($entry))
					{
						$itpos=strpos($lctl, 'pspr.png');
						if($itpos !== false)
							$psp=2;
						else
						{
							$itpos=strpos($lctl, 'psau.png');
							if($itpos !== false)
								$psp=1;
						}
					}
					// Rest
					$entry2='';
					$bold2=false;
					if($GLOBALS['pdf_cal_info_debug'])
						echo('<b>'.$l.' - '.$c.':</b> ['.fxHtmlEncode($lct).']<br />');
					$lct=strtr($lct, array('<br />'=>'<br />', '<BR />'=>'<br />'));
					$brs=explode('<br />', $lct);
					if(stripos($brs[0],'<b>') || stripos($brs[0],'<b ') || stripos($brs[0],'"bold') || stripos($brs[0],'bold"') || stripos($brs[0],' bold') || stripos($brs[0],'bold '))
						$bold=true;
					if(!strlen($entry))
					{
						$entry=fxpdf_convert(strtr(strip_tags($brs[0]), $GLOBALS['trans_html']));
						if(sizeof($brs) > 1)
						{
							$entry2=fxpdf_convert(strtr(strip_tags($brs[1]), $GLOBALS['trans_html']));
							if(stripos($brs[1],'<b>') || stripos($brs[1],'<b '))
								$bold2=true;
						}
					}

//if(($l == 3) && ($c == 0)) $entry='Walter ist lieb, Walter ist lieb, Walter ist lieb, Walter ist lieb, Walter ist lieb, Walter ist lieb, Walter ist lieb, Walter ist lieb, Walter ist lieb, Walter ist lieb, Walter ist lieb, Walter ist lieb, Walter ist lieb, Walter ist lieb.';
					if($GLOBALS['pdf_cal_info_debug'])
					{
						echo('-> <b>'.$entry.'</b><hr>');
						if(strlen($entry2))
							echo('--> <b>'.$entry2.'</b><hr>');
					}
					$len_entry=strlen($entry);
					if($len_entry && ($entry != '~'))
					{
						$GLOBALS['ci'][$l][$c]['text']=$entry;
						$GLOBALS['ci'][$l][$c]['bold']=$bold;
						$GLOBALS['ci'][$l][$c]['colspan']=$colspan;
						$GLOBALS['ci'][$l][$c]['rowspan']=$rowspan;
						if($psp)
							$GLOBALS['ci'][$l][$c]['psp']=$psp;
						if(!isset($GLOBALS['ci']['col'][$c]) || ($len_entry > $GLOBALS['ci']['col'][$c]['len']))
						{
							$GLOBALS['ci']['col'][$c]['len']=$len_entry;
							$GLOBALS['ci']['col'][$c]['line']=$l;
						}

						$len_entry2=strlen($entry2);
						if($len_entry2 && ($entry2 != '~'))
						{
							$GLOBALS['ci'][$l][$c]['text2']=$entry2;
							$GLOBALS['ci'][$l][$c]['bold2']=$bold2;
							if(!isset($GLOBALS['ci']['col'][$c]) || ($len_entry2 > $GLOBALS['ci']['col'][$c]['len']))
							{
								$GLOBALS['ci']['col'][$c]['len']=$len_entry2;
								$GLOBALS['ci']['col'][$c]['line']=$l;
							}
						}
					}
				}
			}

			// Konfigurationsfile einladen und parsen
			$GLOBALS['pdf_array']=load_pdfconfig(-1);
			if($GLOBALS['pdf_cal_info_debug'])
				fxDebug($GLOBALS['ci'], 'ci');
		}
	}
	else if(!is_array($GLOBALS['ci'][0]))
	{
		if($GLOBALS['pdf_cal_info_debug'])
			echo('<B>BREITE ERMITTELN:</B><br />');	// DEL
		if(!$GLOBALS['pdf_array']['fac'])
			$GLOBALS['pdf_array']['fac']=(float)1.0;
		$GLOBALS['pdf_array']['ffac']=0.0;
//fxDebug($GLOBALS['pdf_array'], 'pdf_array');

		// Spaltenhöhe und -breite ermitteln
		$GLOBALS['pdf_array']['fsize']=9.0;
		$GLOBALS['pdf_array']['fheight']=$GLOBALS['pdf_array']['fsize']*$GLOBALS['pdf_array']['fac'];
		$GLOBALS['pdf_array']['cwidth']=6.0*$GLOBALS['pdf_array']['fac'];
		if(isset($GLOBALS['pscal'][2][$GLOBALS['ci']['lcol']]['width']))
			$GLOBALS['pdf_array']['cwidth']	+= ($GLOBALS['pscal'][2][$GLOBALS['ci']['lcol']]['width']-4)/9 * 5.04 * $GLOBALS['pdf_array']['fac'];
		else
			$GLOBALS['pdf_array']['cwidth']	+= fxpdf_strlen($pdf, strip_tags($GLOBALS['pscal'][2][$GLOBALS['ci']['lcol']]['text']), $GLOBALS['pdf_array']['fheight']);
//fxDebug($GLOBALS['pdf_array']['cwidth'], 'cwidth');
		$GLOBALS['pdf_array']['cheight_t']=$GLOBALS['pdf_array']['fheight']+(6.0*$GLOBALS['pdf_array']['fac']);

		if($GLOBALS['ci']['tr'] == 26)		// Sonderfall: Projektstatus
		{
			$GLOBALS['pdf_array']['cheight']=$GLOBALS['pdf_array']['fheight']*2.0 + (3.0*$GLOBALS['pdf_array']['fac']);
			$GLOBALS['pdf_array']['blheight']=($GLOBALS['pdf_array']['cheight']-(4.0*$GLOBALS['pdf_array']['fac']))/2.0;
		}
		else
			$GLOBALS['pdf_array']['cheight']=$GLOBALS['pdf_array']['fheight'] + (3.0*$GLOBALS['pdf_array']['fac']);
//fxDebug($GLOBALS['pdf_array']['cheight'], 'cheight');

		// PDF-Breite der linken Spalten ermitteln
		for($c=$GLOBALS['ci']['scl']; $c<$GLOBALS['ci']['mlcol']; $c++)
		{
			$cl=$GLOBALS['ci']['col'][$c]['line'];
			$txt=$GLOBALS['ci'][$cl][$c]['text'];
			$bld='';
			if(isset($GLOBALS['ci'][$cl][$c]['bold']) && $GLOBALS['ci'][$cl][$c]['bold'])
				$bld='B';
			$txt_len=4.0*$GLOBALS['pdf_array']['fac'] + fxpdf_strlen($pdf, $txt, $GLOBALS['pdf_array']['fheight'], $bld);
			$GLOBALS['ci']['col'][$c]['len']=$txt_len;
			$GLOBALS['ci']['col'][$c]['line']=$txt;

			if(isset($GLOBALS['ci'][$cl][$c]['text2']))
			{
				$txt=$GLOBALS['ci'][$cl][$c]['text2'];
				$bld='';
				if(isset($GLOBALS['ci'][$cl][$c]['bold2']) && $GLOBALS['ci'][$cl][$c]['bold2'])
					$bld='B';
				$txt_len=4.0*$GLOBALS['pdf_array']['fac'] + fxpdf_strlen($pdf, $txt, $GLOBALS['pdf_array']['fheight'], $bld);
				if($txt_len > $GLOBALS['ci']['col'][$c]['len'])
				{
					$GLOBALS['ci']['col'][$c]['len']=$txt_len;
					$GLOBALS['ci']['col'][$c]['line']=$txt;
				}
			}
		}
		if($GLOBALS['pdf_cal_info_debug'])
			fxDebug($GLOBALS['ci'], 'VORHER ('.__LINE__.')'); // DEL

		// Max. Längen der linken Überschriften ermitteln
		do
		{
			$ld_fnd=false;
			for($l=2; $l>=0; --$l)
			{
				$c=$GLOBALS['ci']['scl'];
				while($c < $GLOBALS['ci']['mlcol'])
				{
					$cs=$GLOBALS['pscal'][$l][$c]['colspan'];
					if(!$cs)
						$cs=1;
					$rs=$GLOBALS['pscal'][$l][$c]['rowspan'];
					if(!$rs)
						$rs=1;
					$entry=fxpdf_convert(strtr(strip_tags($GLOBALS['pscal'][$l][$c]['text']), $GLOBALS['trans_html']));
					$len_entry=6.0*$GLOBALS['pdf_array']['fac'] + fxpdf_strlen($pdf, $entry, $GLOBALS['pdf_array']['fheight']+(1.0*$GLOBALS['pdf_array']['fac']), 'B');

					// max. Länge
					$ml=0;
					for($mlc=0; $mlc<$cs; $mlc++)
						$ml += $GLOBALS['ci']['col'][$c+$mlc]['len'];

					if(!fxIsArray($GLOBALS['ci'][$l][$c]))
						$GLOBALS['ci'][$l][$c]=array('text'=>$entry, 'colspan'=>$cs, 'rowspan'=>$rs);
					if($len_entry > $ml)
					{
						$ld_fnd=true;
						$ldiff=($len_entry - $ml) / (float)$cs;
						for($mlc=0; ($mlc<$cs) && ($c+$mlc < $GLOBALS['ci']['mlcol']); $mlc++)
						{
							$GLOBALS['ci']['col'][$c+$mlc]['len'] += $ldiff;
							$ol=$l+1;
							while($ol < 3)
							{
								if($GLOBALS['ci'][$ol][$c+$mlc]['len'])
									$GLOBALS['ci'][$ol][$c+$mlc]['len'] += $ldiff;
								$ol++;
							}
						}
						if($GLOBALS['pdf_cal_info_debug'])
							echo("<B>DIFF</B> in Zeile $l, Spalte $c - ".($c+$cs-1).": $ldiff<br />");	// DEL
						$GLOBALS['ci'][$l][$c]['len']=$len_entry;
					}
					else
						$GLOBALS['ci'][$l][$c]['len']=$ml;

					$c += $cs;
				}
			}
		} while($ld_fnd);

		// Gesamte PDF-Breite der linken Spalten ermitteln
		$gw0=0.0;
		$gw1=0.0;
		for($c=$GLOBALS['ci']['scl']; $c<$GLOBALS['ci']['mlcol']; $c++)
		{
			if($gw0 == 0.0)
				$gw0=$GLOBALS['ci']['col'][$c]['len'];
			else
				$gw1 += $GLOBALS['ci']['col'][$c]['len'];
		}
		$gw=$gw0+$gw1;
//fxDebug($gw,'$gw0='.$gw0.' + $gw1='.$gw1);

		if(!$GLOBALS['pdf_array']['fit'] && ($gw > $GLOBALS['pdf_array']['w_body']))
		{
			$gw=$GLOBALS['pdf_array']['w_body'];
			$gw0=$gw-$gw1;
			$GLOBALS['ci']['col'][$GLOBALS['ci']['scl']]['len']=$gw0;
			for($l=0; $l<3; $l++)
				$GLOBALS['ci'][$l][$GLOBALS['ci']['scl']]['len']=$gw0;
		}

		// Gesamt PDF-Breite inkl. rechter Spalten ermitteln
		$gw += $GLOBALS['ci']['rcol']*$GLOBALS['pdf_array']['cwidth'];
//fxDebug($gw,'$gw0='.$gw0.' + $gw1='.$gw1.' + ($rcol='.$GLOBALS['ci']['rcol'].' * cwidth='.$GLOBALS['pdf_array']['cwidth'].')='.($GLOBALS['ci']['rcol']*$GLOBALS['pdf_array']['cwidth']));

		if($GLOBALS['pdf_array']['fit'] && ($gw > $GLOBALS['pdf_array']['w_body']))
		{
			// ...und neuen Faktor ermitteln
			$nf=floor($GLOBALS['pdf_array']['w_body'])/ceil($gw);

			// ...und PDF anpassen
			$GLOBALS['pdf_array']['fac']=$nf;
			$GLOBALS['pdf_array']['fheight'] *= $nf;
			$GLOBALS['pdf_array']['cwidth'] *= $nf;
			$GLOBALS['pdf_array']['cheight_t'] *= $nf;
			$GLOBALS['pdf_array']['cheight'] *= $nf;
			if(isset($GLOBALS['pdf_array']['blheight']))
				$GLOBALS['pdf_array']['blheight'] *= $nf;
//fxDebug($GLOBALS['pdf_array'], 'pdf_array');

			// ...und Kalender anpassen
			if(sizeof($GLOBALS['ci']['col']))
			{
				foreach($GLOBALS['ci']['col'] as $c => $ca)
					$GLOBALS['ci']['col'][$c]['len'] *= $nf;

				for($l=0; $l<3; $l++)
				{
					foreach($GLOBALS['ci'][$l] as $c => $ca)
						$GLOBALS['ci'][$l][$c]['len'] *= $nf;
				}
			}
		}

		if($GLOBALS['pdf_cal_info_debug'])
			fxDebug($GLOBALS['ci'], 'NACHHER ('.__LINE__.')');
	}
}

/**
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  DPDF_LEFTSIDE                                                            +
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf - Mandatory parameter: ???PARAMETER???
 * @param various $al   - Mandatory parameter: ???PARAMETER???
 * @param various $xc   - Mandatory parameter: ???PARAMETER???
 * @param various $yc   - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dpdf_leftside(&$pdf, $al, $xc, $yc)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($GLOBALS['pdf_function_debug'])
		echo('<div style="margin-top:2px;padding:4px;border:1px solid black;background:#ffffff;">'.__FILE__.' <font class=grey>(Line: '.__LINE__.')</font><div style="padding:3px;border:1px solid black;border-radius:3px;background:#006b9f;"><b class=white>dpdf_leftside</b> <font class=white>(&$pdf, $al='.$al.', $xc='.$xc.', $yc='.$yc.')</font></div>'.$GLOBALS['nl']);

	$sf_200=2.00*$GLOBALS['pdf_array']['fac'];
	$sf_100=1.00*$GLOBALS['pdf_array']['fac'];

	$scl=$GLOBALS['ci']['scl'];
	while(($scl < $GLOBALS['ci']['lcol']) && ($xc < $GLOBALS['pdf_array']['w_body']))
	{
		$ltc=1;
		$ycdiff=0.0;
		$sl=$al;
		while(($sl > 2) && !is_array($GLOBALS['pscal'][$sl][$scl]))
		{
			$sl--;
			$ycdiff += $GLOBALS['pdf_array']['cheight'];
		}
		if(($al != $sl) && !$yc)
			$wl=$sl;
		else
			$wl=$al;

		if(is_array($GLOBALS['ci'][$wl][$scl]) && isset($GLOBALS['ci'][$wl][$scl]['text']) && strlen($GLOBALS['ci'][$wl][$scl]['text']))
		{
			$rs=max(1, (int)$GLOBALS['ci'][$wl][$scl]['rowspan']);
			$ltc=max(1, (int)$GLOBALS['ci'][$wl][$scl]['colspan']);

			if($wl < 3)
			{
				$len=$GLOBALS['ci'][$wl][$scl]['len'];
				$fontsize=$GLOBALS['pdf_array']['fheight']+$sf_100;
				$height=$rs*$GLOBALS['pdf_array']['cheight_t'];
				$theight=$GLOBALS['pdf_array']['fheight']+$sf_200;
				$attr='bold';
			}
			else
			{
//fxDebug($GLOBALS['pscal'][$wl][$scl], 'pscal: $wl='.$wl.', $scl='.$scl);
//fxDebug($GLOBALS['ci'][$wl][$scl], 'ci: $wl='.$wl.', $scl='.$scl);
				$len=$GLOBALS['ci']['col'][$scl]['len'];
				if($ltc > 1)
				{
					for($cs=1; $cs<$ltc; $cs++)
						$len += $GLOBALS['ci']['col'][$scl+$cs]['len'];
				}
				$fontsize=$GLOBALS['pdf_array']['fheight'];
				$height=$rs*$GLOBALS['pdf_array']['cheight'];
				$theight=($height/2.0)+($fontsize/2.0)-$sf_100;
				if($GLOBALS['ci'][$wl][$scl]['psp'] == 2)		// Projekt
					$attr='bold';
				elseif($GLOBALS['ci'][$wl][$scl]['psp'] == 1)	// Aufgabe
					$attr='normal';
				elseif($GLOBALS['ci'][$wl][$scl]['bold'] || (($scl == $GLOBALS['ci']['scl']) && ($GLOBALS['fxpglobals']['tr'] != 26)))	// Bold oder Spalte 1
					$attr='bold';
				else											// Standard
				{
					if(stripos($GLOBALS['pscal'][$wl][$scl]['text'],'<b>') || stripos($GLOBALS['pscal'][$wl][$scl]['text'],'<b '))
						$attr='bold';
					else
						$attr='normal';
				}
			}

			// Absolute Y-Koordinate berechnen
			$ay=$GLOBALS['pdf_array']['min_y']+$yc-$ycdiff;
			if($ay < 0.00)
				$ayc=0.00;
			else
				$ayc=$ay;

			// Farbe ermitteln
			$col='';
			if(!strlen($col) && isset($GLOBALS['pscal'][$wl][$scl]['color']))
				$col=trim($GLOBALS['pscal'][$wl][$scl]['color']);
			if(!strlen($col) && isset($GLOBALS['pscal'][$wl][$scl]['class']) && isset($GLOBALS['style'][$GLOBALS['pscal'][$wl][$scl]['class']]))
				$col=trim($GLOBALS['style'][$GLOBALS['pscal'][$wl][$scl]['class']]);
			if(!strlen($col))
				$col='eeeeee';

			if($GLOBALS['pdf_function_debug'])
				echo('<div style="padding:4px;border:1px solid black;border-top:0;background:#eeeeee;">text="'.fxHtmlEncode($GLOBALS['ci'][$wl][$scl]['text']).'"<br /><font class=blue>$wl='.$wl.', $scl='.$scl.' -- $rs='.$rs.', $ltc='.$ltc.' -- $xc='.$xc.', $yc='.$yc.', $ycdiff='.$ycdiff.', $ay='.$ay.', $ayc='.$ayc.', $len='.$len.', $height='.$height.', $col='.$col.'</font></div>'.$GLOBALS['nl']);

			fxpdf_box($pdf, $xc, '&'.$ayc, $xc+$len, '&'.($ay+$height), $col);

			$ayc=$yc-$ycdiff+$theight;
			if(isset($GLOBALS['ci'][$wl][$scl]['text2']))
				$ayc -= $fontsize*0.5;
			if($ayc > 0.0)
			{
				$txt=fxpdf_strlen($pdf, $GLOBALS['ci'][$wl][$scl]['text'], $fontsize, $attr, $len-$sf_100);
				if(strlen($txt) && ($txt != '~'))
				{
					$fc='000000';
					if(($GLOBALS['fxpglobals']['tr'] == 26) && ($GLOBALS['pscal'][$wl][0]['pid'] < 0))	// Meilenstein
						$fc='987300';
					$align=$GLOBALS['pscal'][$wl][$scl]['align'];
					if($align == 'right')
						$xct=$xc+$len-$sf_100;
					elseif($align == 'center')
						$xct=$xc+($len/2.0);
					else
					{
						$align='left';
						$xct=$xc+$sf_100;
					}
					fxpdf_text($pdf, $txt, $xct, $ayc, $fontsize, $align, $attr, 0.25, $fc);
//echo('<b>fxpdf_text</b>($pdf, '.$txt.', '.$xct.', '.$ayc.', '.$fontsize.', '.$align.', '.$attr.');<br />');

					if(isset($GLOBALS['ci'][$wl][$scl]['text2']))
					{
						$ayc += $fontsize;
						$txt=fxpdf_strlen($pdf, $GLOBALS['ci'][$wl][$scl]['text2'], $fontsize, 'B', $len-$sf_100);
						fxpdf_text($pdf, $txt, $xct, $ayc, $fontsize, $align, 'bold');
//echo('<b>fxpdf_text (2)</b>($pdf, '.$txt.', '.$xct.', '.$ayc.', '.$fontsize.', '.$align.', bold);<br />');
					}
				}
			}

			$xc += $len;
		}
		else
			$xc += $GLOBALS['ci']['col'][$scl]['len'];
		$scl += $ltc;
	}

	if($GLOBALS['pdf_function_debug'])
		echo('</div>'.$GLOBALS['nl']);

	return $xc;
}

/**
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  DPDF_RIGHTSIDE                                                           +
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf - Mandatory parameter:                   ???PARAMETER???
 * @param various $al   - Mandatory parameter:                   ???PARAMETER???
 * @param various $xc   - Mandatory parameter:                   ???PARAMETER???
 * @param various $yc   - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $mode - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dpdf_rightside(&$pdf, $al, $xc, $yc, $mode=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($GLOBALS['pdf_function_debug'])
		echo('<div style="margin-top:2px;padding:4px;border:1px solid black;background:#ffffff;">'.__FILE__.' <font class=grey>(Line: '.__LINE__.')</font><div style="padding:3px;border:1px solid black;border-radius:3px;background:#006b9f;"><b class=white>dpdf_rightside</b> <font class=white>(&$pdf, $al='.$al.', $xc='.$xc.', $yc='.$yc.', $mode='.$mode.')</font></div>'.$GLOBALS['nl']);

	$sf_400=4.00*$GLOBALS['pdf_array']['fac'];
	$sf_250=2.50*$GLOBALS['pdf_array']['fac'];
	$sf_200=2.00*$GLOBALS['pdf_array']['fac'];
	$sf_150=1.50*$GLOBALS['pdf_array']['fac'];
	$sf_100=1.00*$GLOBALS['pdf_array']['fac'];
	$sf_050=0.50*$GLOBALS['pdf_array']['fac'];
	$sf_010=0.10*$GLOBALS['pdf_array']['fac'];

	// Zeile 0: Monat
	$ac=$GLOBALS['acc'];
	$xcdiff=0.0;
	while(($ac >= $GLOBALS['ci']['lcol']) && !is_array($GLOBALS['pscal'][$al][$ac]))
	{
		$ac--;
		$xcdiff += $GLOBALS['pdf_array']['cwidth'];
	}
	// Koordinaten für Abhängigkeiten merken
	if($mode == 'dep')
	{
		$dx1=$GLOBALS['pdf_array']['min_x'] + $xc + ($GLOBALS['pscal'][$al][0]['sk']-$ac)*$GLOBALS['pdf_array']['cwidth'] - $xcdiff;
		$dx2=$dx1 + ($GLOBALS['pscal'][$al][0]['ek']+1-$GLOBALS['pscal'][$al][0]['sk'])*$GLOBALS['pdf_array']['cwidth'];
		$dy1=$GLOBALS['pdf_array']['min_y'] + $yc;
		$dy2=$dy1 + $GLOBALS['pdf_array']['cheight'];
//echo('<b class="red">$al='.$al.', $ac='.$ac.': $dx1='.$dx1.', $dy1='.$dy1.', $dx2='.$dx2.', $dy2='.$dy2.'</b><hr>');
		$GLOBALS['pscal'][$al][0]['coords']=array($dx1,$dy1,$dx2,$dy2);
	}

	while(is_array($GLOBALS['pscal'][$al][$ac]) && (($xc < $GLOBALS['pdf_array']['w_body']) || (($mode == 'dep') && ($al > 2))))
	{
		$colspan=max(1, (int)$GLOBALS['pscal'][$al][$ac]['colspan']);
		$xcs=$xc - $xcdiff;
		$xce=$xcs + $colspan*$GLOBALS['pdf_array']['cwidth'];
		if($al < 3)
		{
			if($al == 2)
				$GLOBALS['lcol']=$ac-$GLOBALS['ci']['lcol'];
			$t=$GLOBALS['pscal'][$al][$ac]['text'];
			$txt='';
			if(strlen($t))
				$txt=fxpdf_strlen($pdf, strip_tags($t), $GLOBALS['pdf_array']['fheight']+1.0, '', $xce-$xcs-1.0);

			// Farbe ermitteln
			$col='';
			if(!strlen($col) && isset($GLOBALS['pscal'][$al][$ac]['color']))
				$col=trim($GLOBALS['pscal'][$al][$ac]['color']);
			if(!strlen($col) && isset($GLOBALS['pscal'][$al][$ac]['class']) && isset($GLOBALS['style'][$GLOBALS['pscal'][$al][$ac]['class']]))
				$col=trim($GLOBALS['style'][$GLOBALS['pscal'][$al][$ac]['class']]);
			if(!strlen($col))
				$col='eeeeee';

			fxpdf_box($pdf, '!'.$xcs, $yc, '!'.$xce, $yc+$GLOBALS['pdf_array']['cheight_t'], $col);

			if(strlen($txt) && ($txt != '~'))
			{
				// Align
				if(isset($GLOBALS['pscal'][$al][$ac]['align']))
					$align=trim($GLOBALS['pscal'][$al][$ac]['align']);
				else if($al < 2)
					$align='left';
				else
					$align='center';
				if($align == 'right')
					$xtc='!'.($xcs + $GLOBALS['pdf_array']['cwidth']*$colspan - $sf_100);
				elseif($align == 'center')
					$xtc='!'.($xcs + $GLOBALS['pdf_array']['cwidth']*$colspan/2.0);
				else
				{
					$align='left';
					$xtc='!'.($xcs + $sf_100);
				}

				// Font-Color
				if(isset($GLOBALS['pscal'][$al][$ac]['font']) && (substr($GLOBALS['pscal'][$al][$ac]['font'], 0, 8) == 'color="#'))
					$fcol=substr($GLOBALS['pscal'][$al][$ac]['font'], 8, 6);
				else
					$fcol='000000';

				fxpdf_text($pdf, $txt, $xtc, $yc+$GLOBALS['pdf_array']['fheight']+$sf_200, $GLOBALS['pdf_array']['fheight']+$sf_100, $align, 'normal', 0.25, $fcol);
			}
		}
		else	// ab Zeile 3: Grafiken zeichnen
		{
			// Hintergrund
			if($mode != 'skip')
			{
				$tm=fxpdf_pst_bg($pdf, '!'.$xcs, $yc, '!'.$xce, $yc+$GLOBALS['pdf_array']['cheight'], $al, $ac, $mode);
				if(($mode != 'mst') && ($mode != 'dep'))
				{
					// Style
					$tstyle='normal';
					// Y-Koordinate
					if($tm)
					{
						$bg_fontsize=$GLOBALS['pdf_array']['cheight']/2.75 - $sf_200;
						if($GLOBALS['ci']['tr'] == 26)		// Sonderfall: Projektstatus
						{
							$bg_yc=$yc + ($GLOBALS['pdf_array']['cheight']/3.0) + $sf_200;
							if($tm > 1)	// Projekt
								$tstyle='bold';
							else
								$bg_yc += $sf_050;
						}
						else
							$bg_yc=$yc + ($GLOBALS['pdf_array']['cheight']/2.0);
					}
					else
					{
						$bg_fontsize=$GLOBALS['pdf_array']['fheight']-$sf_200;
						$bg_yc=$yc + ($GLOBALS['pdf_array']['cheight']/2.0) + ($bg_fontsize/2.0) - 1.0*$GLOBALS['pdf_array']['fac'];
					}
					// X-Koordinate
					if($GLOBALS['ci']['tr'] == 26)		// Sonderfall: Projektstatus
					{
						$bg_xc=$xcs+$sf_100;
						if($tm > 1)	// Projekt
							$bg_xc += $GLOBALS['pdf_array']['cwidth']/4.0;
						else
							$bg_xc += $GLOBALS['pdf_array']['cwidth']/2.0;
						$maxsize=max(0.0, $xce-$xcs-$GLOBALS['pdf_array']['cwidth']-$sf_200);
					}
					else
					{
						$bg_xc=$xcs+$sf_100;
						$maxsize=max(0.0, $xce-$bg_xc);
					}
					// Projektname
					if($maxsize)
					{
	 					$t=$GLOBALS['pscal'][$al][$ac]['itext'];
						if(strlen($t))
						{
//fxDebug($GLOBALS['pscal'][$al][$ac], $al.'-'.$ac);
							$stxt=fxpdf_convert(strtr(strip_tags($t), $GLOBALS['trans_html']));
							$txt=fxpdf_strlen($pdf, $stxt, $bg_fontsize, '', $maxsize);
							if(strlen($txt))
							{
								$nfs=$GLOBALS['pdf_array']['fheight']-$sf_200;
								$ny=$yc + ($GLOBALS['pdf_array']['cheight']/2.0) + ($nfs/2.0) - 0.5*$GLOBALS['pdf_array']['fac'];
								$align='left';
								if(isset($GLOBALS['pscal'][$al][$ac]['align']))
									$align=$GLOBALS['pscal'][$al][$ac]['align'];
								$cs=1;
								if(isset($GLOBALS['pscal'][$al][$ac]['colspan']))
									$cs=(int)$GLOBALS['pscal'][$al][$ac]['colspan'];
								$nx=$xcs+$sf_050;
								if($align == 'center')
									$nx += $GLOBALS['pdf_array']['cwidth']/2;
								else if($align == 'right')
									$nx += $GLOBALS['pdf_array']['cwidth']-$sf_100;
//fxDebug($txt.' ($align='.$align.', $cs='.$cs.', $nx='.$nx.', $ny='.$ny.')', $al.'-'.$ac);
								for($c=0; $c<$cs; $c++)
								{
									fxpdf_text($pdf, $txt, '!'.$nx, $ny, $nfs, $align, $tstyle, $sf_010);
									$nx += $GLOBALS['pdf_array']['cwidth'];
								}
							}
						}
						else
						{
							if($GLOBALS['ci']['tr'] == 264)
							{
								// Farbe ermitteln
								$col='';
								if(!strlen($col) && isset($GLOBALS['pscal'][$al][$ac]['color']))
									$col=trim($GLOBALS['pscal'][$al][$ac]['color']);
								if(!strlen($col) && isset($GLOBALS['pscal'][$al][$ac]['class']) && isset($GLOBALS['style'][$GLOBALS['pscal'][$al][$ac]['class']]))
									$col=trim($GLOBALS['style'][$GLOBALS['pscal'][$al][$ac]['class']]);
								if(strlen($col))
									fxpdf_box($pdf, '!'.$xcs, $yc, '!'.$xce, $yc+$GLOBALS['pdf_array']['cheight'], $col);
							}

							$t=$GLOBALS['pscal'][$al][$ac]['text'];
							$stxt='';
							if(strlen($t))
								$stxt=fxpdf_convert(strtr(strip_tags($t), $GLOBALS['trans_html']));
							if(substr($stxt, 0, 1) == '~')
								$stxt=substr($stxt, 1);
							$txt=fxpdf_strlen($pdf, $stxt, $bg_fontsize, '', $maxsize);
							$sltxt=strlen($txt);
							if($sltxt && ($txt != '~'))
							{
								$sl=fxpdf_strlen($pdf, $txt, $bg_fontsize);
								if(isset($GLOBALS['pscal'][$al][$ac]['align']))
									$align=$GLOBALS['pscal'][$al][$ac]['align'];
								else
									$align='left';
								$nx=$bg_xc+$sf_050;
								$ny=$bg_yc-$sf_050;
								if($align == 'center')
									$nx += $GLOBALS['pdf_array']['cwidth']/2;
								else if($align == 'right')
								{
									$nx += $GLOBALS['pdf_array']['cwidth']-$sf_200;
									$ny -= $sf_050;
								}
//function fxpdf_text(&$pdf, $text, $xc, $yc, $sf=8.0, $align='left', $mode='normal', $lw=0.25, $rgb='000000', $deg=0, $space=0.0)
								fxpdf_text($pdf, $txt, '!'.$nx, $ny, $bg_fontsize, $align, $tstyle, $sf_010);
							}
							else if(strlen($stxt) && ($stxt != '~'))
							{
								$ncl=($ac-$GLOBALS['ci']['lcol'])*$GLOBALS['pdf_array']['cwidth'] - $sf_200;
								$ncr=($GLOBALS['ci']['lcol']+$GLOBALS['ci']['rcol']-$ac-$colspan)*$GLOBALS['pdf_array']['cwidth'] - $sf_200;
//echo("ac=$ac, colspan=$colspan, lcol=".$GLOBALS['ci']['lcol'].", rcol=".$GLOBALS['ci']['rcol']." -> ncl=$ncl, ncr=$ncr<br />");
								if($ncr > $ncl)		// Rechts
								{
									$nc=$ncr;
									$nalign='left';
									$nx=$xce+$sf_200;
								}
								else				// Links
								{
									$nc=$ncl;
									$nalign='right';
									$nx=$xc-$sf_200;
								}
								$txt=fxpdf_strlen($pdf, $stxt, $bg_fontsize, '', $nc);
								if(strlen($txt))
									fxpdf_text($pdf, $txt, '!'.$nx, $bg_yc, $bg_fontsize, $nalign, 'normal', $sf_010);
							}
						}
					}
				}
				if(($mode == 'mst') || ($mode == 'dep'))
					$mode='skip';
			}
		}
		$ac += $colspan;
		$xc=$xce;
		$xcdiff=0.0;
	}

	if($GLOBALS['pdf_function_debug'])
		echo('</div>'.$GLOBALS['nl']);
	return $xc;
}

/**
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  DPDF_MST (Meilensteine)                                                  +
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf - Mandatory parameter: ???PARAMETER???
 * @param various $x1   - Mandatory parameter: ???PARAMETER???
 * @param various $y1   - Mandatory parameter: ???PARAMETER???
 * @param various $z    - Mandatory parameter: ???PARAMETER???
 * @param various $s    - Mandatory parameter: ???PARAMETER???
 */
function dpdf_mst(&$pdf, $x1, $y1, $z, $s)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Meilensteine
	if(fxIsArray($GLOBALS['pscal'][$z][0]['mst']))
	{
//echo('<b style="color:#ffffff;">Meilensteine in Zeile '.$z.', Startspalte ('.$s.')</b><br />');
		$sf_100=1.00*$GLOBALS['pdf_array']['fac'];
		$sf_200=2.00*$GLOBALS['pdf_array']['fac'];

		foreach($GLOBALS['pscal'][$z][0]['mst'] as $mc => $mst)
		{
//fxDebug($mst,'$mst: $z='.$z.', $mc='.$mc);
			$fontsize=$GLOBALS['pdf_array']['cheight']/3.0;
			$md=$GLOBALS['pdf_array']['cwidth']/3.0;

			$sx=$x1 + ($mst['col'] - $s + 0.5)*$GLOBALS['pdf_array']['cwidth'];
			$sy=$y1 + 0.5*$GLOBALS['pdf_array']['cheight'];

			$ul=$sx-$md;
			$ur=$sx+$md;
			$ut=$sy-$md;
			$ub=$sy+$md;
//echo('<div style="margin-top:2px;padding:4px;border:1px solid black;background:#ffffff;">'.__FILE__.' <font class=grey>(Line: '.__LINE__.')</font><div style="padding:3px;border:1px solid black;border-radius:3px;background:#006b9f;"><b class=white>dpdf_mst</b> <font class=white>Name: "'.$mst['name'].'" (col='.$mst['col'].', x1='.$x1.', $y1='.$y1.', $z='.$z.', $s='.$s.' -- sx='.$sx.', $sy='.$sy.', -- $ul='.$ul.', $ur='.$ur.', $ut='.$ut.', $ub='.$ub.')</font></div></div>'.$GLOBALS['nl']);

			// Verknüpfungslinie (nicht für feste Meilensteine)
			if($mst['type'])
			{
				$cy=$ut-$sf_100;
				$vx=$sx;
				$dd=$mst['col']-$mst['px'];
				if($dd)
				{
//function fxpdf_line(&$pdf, $x1, $y1, $x2, $y2, $wd=0.0, $bd=0.0, $lw=0.35, $rgb='bbbbbb')
					$cx=$sx;
					$vx=0.0;
					$dx=abs($dd)*$GLOBALS['pdf_array']['cwidth'];
					if($dd > 0)
					{
						$cx -= $dx;
						if($mst['type'] == -1)
							$cx -= $GLOBALS['pdf_array']['cwidth']/4.0;
						else
							$cx += $GLOBALS['pdf_array']['cwidth']/4.0;
						if($cx < $x1)
							$cx=$x1+$sf_100;
						else
							$vx=$cx;
						fxpdf_line($pdf, '§'.$cx, '§'.$cy, '§'.$sx, '§'.$cy, 2.0*$GLOBALS['pdf_array']['fac'], 2.0*$GLOBALS['pdf_array']['fac'], 0.20*$GLOBALS['pdf_array']['fac'], 'de7e00');
					}
					else
					{
						$cx += $dx;
						if($mst['type'] == -1)
							$cx -= $GLOBALS['pdf_array']['cwidth']/4.0;
						else
							$cx += $GLOBALS['pdf_array']['cwidth']/4.0;
						if($cx > $x1+$GLOBALS['ci']['rcol']*$GLOBALS['pdf_array']['cwidth'])
							$cx=$x1+$GLOBALS['ci']['rcol']*$GLOBALS['pdf_array']['cwidth']-$sf_100;
						else
							$vx=$cx;
						fxpdf_line($pdf, '§'.$sx, '§'.$cy, '§'.$cx, '§'.$cy, 2.0*$GLOBALS['pdf_array']['fac'], 2.0*$GLOBALS['pdf_array']['fac'], 0.20*$GLOBALS['pdf_array']['fac'], 'de7e00');
					}
				}
				if($vx > 0.0)
					fxpdf_line($pdf, '§'.$vx, '§'.($cy-(($mc+1)*$GLOBALS['pdf_array']['cheight'])+$GLOBALS['pdf_array']['cheight']/4.0), '§'.$vx, '§'.$cy, 2.0*$GLOBALS['pdf_array']['fac'], 2.0*$GLOBALS['pdf_array']['fac'], 0.20*$GLOBALS['pdf_array']['fac'], 'de7e00');
			}

			// Meilenstein zeichnen
			fxpdf_pst_mst_draw($pdf, $sx, $ul, $ur, $ut, $ub, 0.0, $mst['type']);

			$stxt=fxpdf_convert(strtr(strip_tags($mst['name']), $GLOBALS['trans_html']));
			$mw=($GLOBALS['ci']['lcol']+$GLOBALS['ci']['rcol']-$s)*$GLOBALS['pdf_array']['cwidth'] - 3.0*$GLOBALS['pdf_array']['fac'];
			$txt=fxpdf_strlen($pdf, $stxt, $fontsize, '', $mw);
			if(strlen($txt))
			{
				$sdx=$ul+$GLOBALS['pdf_array']['cwidth'];
				$sdy=$sy+$fontsize/2.0-$sf_100;
				fxpdf_text($pdf, $txt, '§'.$sdx, '§'.$sdy, $fontsize, 'left', 'normal', 0.25*$GLOBALS['pdf_array']['fac'], '987300');
			}
		}
	}
}
?>