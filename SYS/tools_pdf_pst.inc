<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : tools_pdf_pst.inc                                            //
// Version     : 24.1                                                         //
// Begin       : 2020-11-19                                                   //
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
 * Special graphical PDF function collection
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  FXPDF_PST_BG (Background)                                                 +
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf - Mandatory parameter:                   ???PARAMETER???
 * @param various $x1   - Mandatory parameter:                   ???PARAMETER???
 * @param various $y1   - Mandatory parameter:                   ???PARAMETER???
 * @param various $x2   - Mandatory parameter:                   ???PARAMETER???
 * @param various $y2   - Mandatory parameter:                   ???PARAMETER???
 * @param various $z    - Mandatory parameter:                   ???PARAMETER???
 * @param various $s    - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $bg   - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fxpdf_pst_bg(&$pdf, $x1, $y1, $x2, $y2, $z, $s, $bg=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($bg === false)
	{
		$bg=$GLOBALS['pscal'][$z][$s]['background'];
		if($GLOBALS['pscal'][$z][$s]['typ'] == 'nv')
			$bg='pr999';
	}

	$sf_200=2.00*$GLOBALS['pdf_array']['fac'];
	$sf_175=1.75*$GLOBALS['pdf_array']['fac'];
	$sf_150=1.50*$GLOBALS['pdf_array']['fac'];
	$sf_100=1.00*$GLOBALS['pdf_array']['fac'];
	$sf_075=0.75*$GLOBALS['pdf_array']['fac'];
	$sf_050=0.50*$GLOBALS['pdf_array']['fac'];
	$sf_040=0.40*$GLOBALS['pdf_array']['fac'];
	$sf_025=0.25*$GLOBALS['pdf_array']['fac'];
	$sf_020=0.20*$GLOBALS['pdf_array']['fac'];
	$sf_010=0.10*$GLOBALS['pdf_array']['fac'];

	$x1=fxpdf_getx($x1);
	$x2=fxpdf_getx($x2);
	if($x2 < $x1)
		$x2=$x1;

	$y1=fxpdf_gety($y1);
	$y2=fxpdf_gety($y2);
	if($y2 < $y1)
		$y2=$y1;
//echo('<b>['.$z.','.$s.']</b> fxpdf_pst_bg(&$pdf, $x1='.$x1.', $y1='.$y1.', $x2='.$x2.', $y2='.$y2.', $bg='.$bg.')<br>');

	if(!is_array($bg))
		$bga[0]=$bg;
	else
		$bga=$bg;

	$first=true;
	$msr=array();
	foreach($bga as $bg)
	{
		if(!$first)
		{
			$x1=$x2;
			$x2=$x1+$GLOBALS['pdf_array']['cwidth'];
		}

		$first=false;
		$mst='';
		$dep='';
		if($bg == 'mst')
			$mst=$bg;
		if($bg == 'dep')
			$dep=$bg;
//echo('$bg='.$bg.', $mst='.$mst.', $dep='.$dep.'<br>');

		if((is_null($bg) || !strlen((string)$bg)) && ($GLOBALS['ci']['tr'] != 92))
			continue;

		$tooltip	= '';
		$rgb1		= 'ffffff';
		$rgb2		= '';
		$lrgb1		= '';
		$lrgb2		= '';
		$ym			= $y2;
		$tm			= 0;		// Text-Mode: 0=normal, 1=hochgestellt Aufgabe, 2=hochgestellt Projekt

		if($GLOBALS['ci']['tr'] == 92)
			$rgb1='f4f4f4';

		// Projektstatus (Gantt)
		if($GLOBALS['ci']['tr'] == 26)
		{
			// Koordinaten
			$plx1=$x1;
			$plx2=$x1+$GLOBALS['pdf_array']['cwidth'];
			$plxm=$plx1+($plx2-$plx1)/2.0;

			$prx1=$x2-$GLOBALS['pdf_array']['cwidth'];
			$prx2=$x2;
			$prxm=$prx1+($prx2-$prx1)/2.0;

			$py1=$y1+$sf_200;
			$pym=$py1 + $GLOBALS['pdf_array']['blheight'];
			$py2=$pym;

			if(!strlen((string)$mst) && !strlen((string)$dep))
			{
				// Projektart ermitteln: Projekt (=0) oder Aufgabe (=1)
				$pt=$GLOBALS['pscal'][$z][0]['pt'];
				$psau=0;
				if(substr((string)$pt,0,1) == 't')
					$psau=1;

				// Mode ermitteln: 0=leer, 1=Soll, 2=Ist + 3=Soll+Ist
				$md=$GLOBALS['pscal'][$z][$s]['mode'];
				$mode_prev=(int)substr((string)$md, 0, 1);
				$mode=(int)substr((string)$md, 1, 1);
				$mode_next=(int)substr((string)$md, 2, 1);

				// Status ermitteln
				$idc=array(297=>'gep', 299=>'ina', 300=>'akt', 301=>'abg');
				$status=(int)$GLOBALS['pscal'][$z][$s]['status'];
				$statcol=$GLOBALS['bstyle']['ps'.$idc[$status]];

				// Colspan
				$colspan=max(1, (int)$GLOBALS['pscal'][$z][$s]['colspan']);

				// Kategoriefarbe ermitteln
				$cb=$GLOBALS['pscal'][$z][0]['cb'];
//echo($z.': $pt='.$pt.', $cb='.$cb.'<br>');

				// Textmode: 1=Aufgabe, 2=Projekt
				$tm=1;
				if(!$psau)
					$tm=2;
//echo('$mode_prev='.$mode_prev.', $mode='.$mode.', $mode_next='.$mode_next.' -- $psau='.$psau.', $status='.$status.', $tm='.$tm.'<br>');

				// Sollbalken
				if(($mode == 1) || ($mode == 3))
				{
					$bx1=$plx1;
					if(!$mode_prev || ($mode_prev == 2))
						$bx1 += $sf_200;

					$bx2=$prx2;
					if(!$mode_next || ($mode_next == 2))
						$bx2 -= $sf_200;

					fxpdf_pst_sollbalken($pdf, $psau, $bx1, $bx2, $py1, $pym, 0.0, $md, $cb);
				}

				// Startsollknoten
				$pkl=$GLOBALS['pscal'][$z][$s]['pkl'];
				if($pkl)
					fxpdf_pst_sollpk($pdf, $psau, true, $plx1+$sf_100, $plxm, $plx2-$sf_100, $py1, $pym, $py2, $statcol, 0.0, $status);

				// Endsollknoten
				$pkr=$GLOBALS['pscal'][$z][$s]['pkr'];
				if($pkr)
					fxpdf_pst_sollpk($pdf, $psau, false, $prx1+$sf_100, $prxm, $prx2-$sf_100, $py1, $pym, $py2, $statcol, 0.0, $status);

				// Istbalken
				if(($mode == 2) || ($mode == 3))
				{
					$bx1=$plx1;
					if(!$mode_prev || ($mode_prev == 1))
						$bx1 += $sf_175;

					$bx2=$prx2;
					if(!$mode_next || ($mode_next == 1))
						$bx2 -= $sf_175;

					fxpdf_pst_istbalken($pdf, $mode, $bx1, $bx2, $py1, $pym);
				}

				// Startistknoten
				$ikl=$GLOBALS['pscal'][$z][$s]['ikl'];
				if($ikl)
					fxpdf_pst_istpk($pdf, $psau, true, $plx1+$sf_100, $plxm, $plx2-$sf_100, $py1, $pym);

				// Endistknoten
				$ikr=$GLOBALS['pscal'][$z][$s]['ikr'];
				if($ikr)
					fxpdf_pst_istpk($pdf, $psau, false, $prx1+$sf_100, $prxm, $prx2-$sf_100, $py1, $pym);

				// Startmarkierung ohne Knoten
				if(!$pkl && !$ikl && !$mode_prev)
				{
					fxpdf_linestyle($pdf, 0.5, 0.5, 0.25);
					fxpdf_color($pdf, 'D', 0, '000000');
					$pdf->Line($plx1+$sf_100, $py1, $plx1+$sf_100, $pym);
				}

				// Endmarkierung ohne Knoten
				if(!$pkr && !$ikr && !$mode_next)
				{
					fxpdf_linestyle($pdf, 0.5, 0.5, 0.25);
					fxpdf_color($pdf, 'D', 0, '000000');
					$pdf->Line($prx2-$sf_100, $py1, $prx2-$sf_100, $pym);
				}
			}

			// Abhängigkeiten
			else if(strlen((string)$dep))
				fxpdf_pst_dep($pdf, $GLOBALS['pscal'][$z][0]['coords'][0], $GLOBALS['pscal'][$z][0]['coords'][1], $GLOBALS['pscal'][$z][0]['coords'][2], $GLOBALS['pscal'][$z][0]['coords'][3], $z, $s);

			// Meilensteine
			else
				dpdf_mst($pdf, $x1, $y1, $z, $s);
		}

		// Rest
		else
		{
			$rgba=fxpdf_pst_bgcol($bg, $y1, $y2, $ym);
			extract($rgba, EXTR_OVERWRITE);
//fxDebug($rgba,'$bg='.$bg, 0);

			// ##### Verfügbarkeit: Tooltip Auslastung #####
			if((substr((string)$bg,0,2) == 'au') || (substr((string)$bg,0,2) == 'ap'))
			{
				if(isset($GLOBALS['pscal'][$z][$s]['tooltip']) && (substr((string)$GLOBALS['pscal'][$z][$s]['tooltip'], -1) == '%'))
					$tooltip=$GLOBALS['pscal'][$z][$s]['tooltip'];
				else if(isset($GLOBALS['pscal'][$z][$s]['tooltip_js']) && (substr((string)$GLOBALS['pscal'][$z][$s]['tooltip_js'], -1) == '%'))
					$tooltip=$GLOBALS['pscal'][$z][$s]['tooltip_js'];
			}
//echo('$rgb1:'.$rgb1.', $rgb2:'.$rgb2.', $lrgb1:'.$lrgb1.', $lrgb2:'.$lrgb2.', $x1:'.$x1.', $y1:'.$y1.', $x2:'.$x2.', $y2:'.$y2.', $ym:'.$ym.'<br>');

			// Hauptfarbe 1
			fxpdf_linestyle($pdf, 0.0, 0.0, 0.10);
			fxpdf_color($pdf, 'DF', 0, $rgb1);
			$pdf->Rect($x1+$sf_010, $y1+$sf_010, $x2-$x1-$sf_020, $ym-$y1-$sf_010, 'DF');
			// Farbe gestreift 1
			if(strlen((string)$lrgb1))
			{
				fxpdf_linestyle($pdf, 0.0, 0.0, 0.75);
				fxpdf_color($pdf, 'S', 0, $lrgb1);
				for($y=$y1+1.50; $y<$ym; $y+=$sf_150)
					$pdf->Line($x1+$sf_050, $y, $x2-$sf_050, $y);
			}
			// Hauptfarbe 2
			if(strlen((string)$rgb2))
			{
				fxpdf_linestyle($pdf, 0.0, 0.0, 0.10);
				fxpdf_color($pdf, 'DF', 0, $rgb2);
				$pdf->Rect($x1+$sf_010, $ym+$sf_010, $x2-$x1-$sf_020, $y2-$ym-$sf_020, 'DF');
				// Farbe gestreift 2
				if(strlen((string)$lrgb2))
				{
					fxpdf_linestyle($pdf, 0.0, 0.0, 0.75);
					fxpdf_color($pdf, 'S', 0, $lrgb2);
					for($y=$ym+1.50; $y<$y2; $y+=$sf_150)
						$pdf->Line($x1+$sf_050, $y, $x2-$sf_050, $y);
				}
			}
		}

		// Tooltip: Auslastung
		if($tooltip && ((int)$tooltip > 0))
		{
			$tooltip=(int)$tooltip;
			$rgbcol='000000';
			$tooltip .= '%';
			$ttfs=4.00*$GLOBALS['pdf_array']['fac'];
			fxpdf_text($pdf, $tooltip, '&'.($x1+($GLOBALS['pdf_array']['cwidth']/2.0)), '&'.($y2-$ttfs), $ttfs, 'center', 'normal', 0.25, $rgbcol);
		}
	}

	return $tm;
}

/**
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  FXPDF_PST_BGCOL (Background-Color)                                       +
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various $bg - Mandatory parameter:                 ???PARAMETER???
 * @param double  $y1 - Optional parameter (default = 0.0):  ???PARAMETER???
 * @param double  $y2 - Optional parameter (default = 0.0):  ???PARAMETER???
 * @param double  $ym - Optional parameter (default = 0.0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fxpdf_pst_bgcol($bg, $y1=0.0, $y2=0.0, $ym=0.0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$rgb1='ffffff';
	$rgb2='';
	$lrgb1='';
	$lrgb2='';

	if($GLOBALS['ci']['tr'] == 92)
		$rgb1='f4f4f4';

	// ##### Verfügbarkeit: Auslastung #####
	if(substr((string)$bg,0,2) == 'au')
	{
		$rgb1='dedede';
		$id=(int)substr((string)$bg,2);
		if(substr((string)$bg,2) == '_ud')
			$lrgb1='b5b5b5';
		else if(substr((string)$bg,2) == '_ft')		// Feiertag
			$lrgb1='edeb6d';
		else if(substr((string)$bg,2) == '_n')		// Nicht verfügbar
			$lrgb1='9fdfff';
		else if(substr((string)$bg,2) == '_x')		// Überlastet
			$rgb1='e1001a';
		else if($id > 0)
		{
			if($id < 100)					// Auslastung 0<x<100
			{
				$rgb2='b5b5b5';
				$ym=$y2-(($y2-$y1)*($id/100.0));
			}
			else if($id == 100)				// Auslastung 100
				$rgb1='b5b5b5';
			else if($id < 200)				// Auslastung 100<x<200
			{
				$rgb1='b5b5b5';
				$rgb2='b5b5b5';
				$lrgb2='e1001a';
				$ym=$y2-(($y2-$y1)*(($id-100.0)/100.0));
			}
			else if($id == 200)				// Auslastung 200
			{
				$rgb1='b5b5b5';
				$lrgb1='e1001a';
			}
			else if($id > 1000)				// Sondertage
			{
				$rgb1=$GLOBALS['bstyle']['t'.$id];
				// Halbe Sondertage
				if(substr((string)$bg,-2) == '_2')
				{
					$ym=$y2-(($y2-$y1)/2.0);
				}
				// Anträge
				if($GLOBALS['starr'][$id])
					$lrgb1='dedede';
			}
			else
				$rgb1='e1001a';				// Überlastet
		}
	}

	// ##### Verfügbarkeit: Auslastung Vergangenheit 0<x<100 #####
	else if(substr((string)$bg,0,2) == 'ap')
	{
		$rgb1='dedede';
		$id=(int)substr((string)$bg,2);
		if($id > 0)
		{
			$rgb2='b5b5b5';
			$ym=$y2-(($y2-$y1)*($id/100.0));
		}
	}

	// ##### Verfügbarkeit: Projektauslastung #####
	else if(substr((string)$bg,0,2) == 'pr')
	{
		$id=(int)substr((string)$bg, 2);
		if(substr((string)$bg,2) == '_ub')			// Überlastet
		{
			$rgb1='3d8cb3';
			$lrgb1='e1001a';
		}
		else if($id == 100)					// Auslastung 100
			$rgb1='3d8cb3';
		else if($id < 100)					// Auslastung 0<x<100
		{
			$rgb1='dedede';
			if($id > 0)
			{
				$rgb2='3d8cb3';
				$ym=$y2-(($y2-$y1)*($id/100.0));
			}
		}
		else if($GLOBALS['ci']['tr'] == 92)
			$rgb1='ffffff';
		else
			$rgb1='eeeeee';
	}
//echo('$rgb1:'.$rgb1.', $rgb2:'.$rgb2.', $lrgb1:'.$lrgb1.', $lrgb2:'.$lrgb2.' -- $ym='.$ym.'<br />');

	return array('rgb1'=>$rgb1, 'rgb2'=>$rgb2, 'lrgb1'=>$lrgb1, 'lrgb2'=>$lrgb2, 'ym'=>$ym);
}

/**
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  FXPDF_PST_SOLLBALKEN (Verbindungsbalken zwischen Anfangs- und Endpunkt)   +
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf - Mandatory parameter:                   ???PARAMETER???
 * @param various $psau - Mandatory parameter:                   ???PARAMETER???
 * @param various $x1   - Mandatory parameter:                   ???PARAMETER???
 * @param various $x2   - Mandatory parameter:                   ???PARAMETER???
 * @param various $y1   - Mandatory parameter:                   ???PARAMETER???
 * @param various $y2   - Mandatory parameter:                   ???PARAMETER???
 * @param double  $fac  - Optional parameter (default = 0.0):    ???PARAMETER???
 * @param string  $md   - Optional parameter (default = '000'):  ???PARAMETER???
 * @param string  $cb   - Optional parameter (default = ''):     ???PARAMETER???
 */
function fxpdf_pst_sollbalken(&$pdf, $psau, $x1, $x2, $y1, $y2, $fac=0.0, $md='000', $cb='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($fac < 0.0)
		$fac=-$fac;
	else if($fac == 0.0)
		$fac=$GLOBALS['pdf_array']['fac'];
	else
		$fac *= $GLOBALS['pdf_array']['fac'];

	$sf_200=2.00*$fac;
	$sf_175=1.75*$fac;
	$sf_150=1.50*$fac;
	$sf_125=1.25*$fac;
	$sf_100=1.00*$fac;
	$sf_075=0.75*$fac;
	$sf_050=0.50*$fac;
	$sf_025=0.25*$fac;

	$mode_prev	= (int)substr((string)$md, 0, 1);
	$mode		= (int)substr((string)$md, 1, 1);
	$mode_next	= (int)substr((string)$md, 2, 1);

	fxpdf_linestyle($pdf, 0.0, 0.0, -$sf_050);

	if(!is_null($cb))
		$cb=trim((string)$cb);
	else
		$cb='';

	// Aufgabe
	if($psau)
	{
		if(strlen((string)$cb))
		{
			if(substr((string)$cb,0,1) == '#')
				$cb=substr((string)$cb,1);
			if(strlen((string)$cb) != 6)
				$cb='bbbbbb';
		}
		else
			$cb='bbbbbb';
		if($cb == 'dcdcdc')
			$cb='aaaaaa';
//echo('<b>Task:</b> $cb='.$cb.'<br>');

		$yh=($y2-$y1)/2.0;
		$yo=$yh/6.0;
		$yu=$yh/8.0;
//echo('$y1='.$y1.', $y2='.$y2.', $yh='.$yh.', $yo='.$yo.', $yu='.$yu.'<br>');

		// Grundbalken oben
		$co=fxpdf_lighten(24,$cb);
		fxpdf_color($pdf, 'DF', 0, $co);	// e8e8e8
		$pdf->Rect($x1, $y1, $x2-$x1, $yh, 'DF');

		// Abstufungen oben
		for($a=1; $a<=6; $a++)
		{
			$ca=fxpdf_lighten($a*5,$co);
			fxpdf_color($pdf, 'DF', 0, $ca);
			$pdf->Rect($x1, $y1+$yh-$a*$yo, $x2-$x1, $yo, 'DF');
		}

		// Grundbalken unten
		$cu=fxpdf_lighten(8,$cb);
		fxpdf_color($pdf, 'DF', 0, $cu);	// d4d4d4
		$pdf->Rect($x1, $y1+$yh, $x2-$x1, $yh, 'DF');

		// Abstufungen unten
		for($a=1; $a<=6; $a++)
		{
			$ca=fxpdf_lighten($a*-4,$cu);
			fxpdf_color($pdf, 'DF', 0, $ca);
			$pdf->Rect($x1, $y1+$yh+$a*$yu, $x2-$x1, $yu, 'DF');
		}
		$ca=fxpdf_lighten(-54,$cu);
		fxpdf_color($pdf, 'DF', 0, $ca);
		$pdf->Rect($x1, $y1+$yh+7*$yu, $x2-$x1, $yu, 'DF');
	}
	// Projekt
	else
	{
		if(strlen((string)$cb))
		{
			if(substr((string)$cb,0,1) == '#')
				$cb=substr((string)$cb,1);
			if(strlen((string)$cb) != 6)
				$cb='';
			if($cb == 'dcdcdc')
				$cb='aaaaaa';
		}
//echo('<b>Project:</b> $cb='.$cb.'<br>');

		$yu=($y2-$y1-$sf_100)/14.0;

		// Grundbalken
		$cu='eeeeee';
		fxpdf_color($pdf, 'DF', 0, $cu);
		$pdf->Rect($x1, $y1, $x2-$x1, $y2-$y1-$sf_100, 'DF');

		// Abstufungen unten
		for($a=1; $a<=13; $a++)
		{
			$ca=fxpdf_lighten($a*-8,$cu);
			fxpdf_color($pdf, 'DF', 0, $ca);
			$pdf->Rect($x1, $y1+$a*$yu, $x2-$x1, $yu, 'DF');
		}

		// Linien oben
		if(!strlen((string)$cb))
			fxpdf_color($pdf, 'S', 0, '787878');
		else
			fxpdf_color($pdf, 'S', 0, $cb);
		$pdf->Line($x1-$sf_025, $y1, $x2+$sf_025, $y1);
		$pdf->Line($x1-$sf_025, $y1+$sf_025, $x2+$sf_025, $y1+$sf_025);
		if(strlen((string)$cb))
		{
			$ca=fxpdf_lighten(-40,$cb);
			fxpdf_color($pdf, 'S', 0, $ca);
		}
		$pdf->Line($x1-$sf_025, $y1+$sf_050, $x2+$sf_025, $y1+$sf_050);
	}
}

/**
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  FXPDF_PST_ISTBALKEN (Verbindungsbalken zwischen Anfangs- und Endpunkt)    +
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf - Mandatory parameter:                 ???PARAMETER???
 * @param various $mode - Mandatory parameter:                 ???PARAMETER???
 * @param various $x1   - Mandatory parameter:                 ???PARAMETER???
 * @param various $x2   - Mandatory parameter:                 ???PARAMETER???
 * @param various $y1   - Mandatory parameter:                 ???PARAMETER???
 * @param various $y2   - Mandatory parameter:                 ???PARAMETER???
 * @param double  $fac  - Optional parameter (default = 0.0):  ???PARAMETER???
 */
function fxpdf_pst_istbalken(&$pdf, $mode, $x1, $x2, $y1, $y2, $fac=0.0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($fac < 0.0)
		$fac=-$fac;
	else if($fac == 0.0)
		$fac=$GLOBALS['pdf_array']['fac'];
	else
		$fac *= $GLOBALS['pdf_array']['fac'];

	$sf_250=2.50*$fac;
	$sf_150=1.50*$fac;
	$sf_100=1.00*$fac;
	$sf_075=0.75*$fac;
	$sf_050=0.50*$fac;
	$sf_010=0.10*$fac;

	$ym=$y1+(($y2-$y1)/3.0);

	fxpdf_linestyle($pdf, 0.0, 0.0, -$sf_050);

	// Farben
	$rgbh='00ffae';
	$rgbd='009f6b';

	// Linien
	fxpdf_color($pdf, 'DF', 0, $rgbh);
	$pdf->Rect($x1-$sf_075, $ym-$sf_250, $x2-$x1+$sf_100, $sf_075, 'DF');

	fxpdf_color($pdf, 'S', 0, $rgbd);
	$pdf->Rect($x1-$sf_075, $ym-$sf_150, $x2-$x1+$sf_100, $sf_010, 'DF');
}

/**
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  FXPDF_PST_SOLLPK (Anfangs- bzw. Endpunkt)                                 +
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf   - Mandatory parameter:                 ???PARAMETER???
 * @param various $psau   - Mandatory parameter:                 ???PARAMETER???
 * @param various $left   - Mandatory parameter:                 ???PARAMETER???
 * @param various $x1     - Mandatory parameter:                 ???PARAMETER???
 * @param various $xm     - Mandatory parameter:                 ???PARAMETER???
 * @param various $x2     - Mandatory parameter:                 ???PARAMETER???
 * @param various $y1     - Mandatory parameter:                 ???PARAMETER???
 * @param various $ym     - Mandatory parameter:                 ???PARAMETER???
 * @param various $y2     - Mandatory parameter:                 ???PARAMETER???
 * @param various $crgb   - Mandatory parameter:                 ???PARAMETER???
 * @param double  $fac    - Optional parameter (default = 0.0):  ???PARAMETER???
 * @param integer $status - Optional parameter (default = 297):  ???PARAMETER???
 */
function fxpdf_pst_sollpk(&$pdf, $psau, $left, $x1, $xm, $x2, $y1, $ym, $y2, $crgb, $fac=0.0, $status=297)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$l1=-20;
	$l2=40;

	if($fac < 0.0)
		$fac=-$fac;
	else if($fac == 0.0)
		$fac=$GLOBALS['pdf_array']['fac'];
	else
		$fac *= $GLOBALS['pdf_array']['fac'];

	$sf_100=1.00*$fac;
	$sf_075=0.75*$fac;
	$sf_050=0.50*$fac;
	$sf_025=0.25*$fac;

	fxpdf_linestyle($pdf, 0.0, 0.0, -$sf_050);

	if(!$psau)	// Projekt
	{
		fxpdf_color($pdf, 'DF', 0, '787878');
		if($left)
			$pdf->Rect($x1, $ym+$sf_100, $sf_075, $y1-$ym-$sf_100, 'DF');
		else
			$pdf->Rect($x2-$sf_075, $ym+$sf_100, $sf_075, $y1-$ym-$sf_100, 'DF');
	}
	else	// Aufgabe
	{
		$crgba=array(FXP_PS_PLANNED=>'dddddd', FXP_PS_INACTIVE=>'aaaaaa', FXP_PS_ACTIVE=>'777777', FXP_PS_COMPLETED=>'444444');
		if(isset($crgba[$status]))
			$crgb=$crgba[$status];
		else
			$crgb='e1001a';
		$frgb=$crgb;
		$sf=0.00;
		if(($status == FXP_PS_ACTIVE) || ($status == FXP_PS_COMPLETED))
		{
			if($status == FXP_PS_ACTIVE)
				$frgb='333333';
			else
				$frgb='000000';
			$frame=true;
			$sf=$sf_050;
		}

		if($left)
		{
			$nx1=$x1;
			$nx2=$xm;
		}
		else
		{
			$nx1=$xm;
			$nx2=$x2;
		}

		// Aufgabe: Rechteck
		fxpdf_color($pdf, 'F', 0, $crgb);
		fxpdf_color($pdf, 'S', 0, $frgb);
		$pdf->Rect($nx1, $ym, $nx2-$nx1, $y1-$ym, 'DF');

		// Helle Seite, links oben
		fxpdf_color($pdf, 'S', $l2, $crgb);
		$pdf->MoveTo($nx1+$sf, $ym-$sf);
		$pdf->LineTo($nx1+$sf, $y1+$sf);
		$pdf->LineTo($nx2-$sf, $y1+$sf, 'D');

		// Dunkle Seite, rechts unten
		fxpdf_color($pdf, 'S', $l1, $crgb);
		$pdf->MoveTo($nx2-$sf, $y1+$sf+$sf_025);
		$pdf->LineTo($nx2-$sf, $ym-$sf);
		$pdf->LineTo($nx1+$sf+$sf_025, $ym-$sf, 'D');
	}
}

/**
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  FXPDF_PST_ISTPK (Anfangs- bzw. Endpunkt)                                  +
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf - Mandatory parameter:                 ???PARAMETER???
 * @param various $psau - Mandatory parameter:                 ???PARAMETER???
 * @param various $left - Mandatory parameter:                 ???PARAMETER???
 * @param various $x1   - Mandatory parameter:                 ???PARAMETER???
 * @param various $xm   - Mandatory parameter:                 ???PARAMETER???
 * @param various $x2   - Mandatory parameter:                 ???PARAMETER???
 * @param various $y1   - Mandatory parameter:                 ???PARAMETER???
 * @param various $y2   - Mandatory parameter:                 ???PARAMETER???
 * @param double  $fac  - Optional parameter (default = 0.0):  ???PARAMETER???
 */
function fxpdf_pst_istpk(&$pdf, $psau, $left, $x1, $xm, $x2, $y1, $y2, $fac=0.0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($fac < 0.0)
		$fac=-$fac;
	else if($fac == 0.0)
		$fac=$GLOBALS['pdf_array']['fac'];
	else
		$fac *= $GLOBALS['pdf_array']['fac'];

	$sf_300=3.00*$fac;
	$sf_275=2.75*$fac;
	$sf_250=2.50*$fac;
	$sf_125=1.25*$fac;
	$sf_100=1.00*$fac;
	$sf_075=0.75*$fac;
	$sf_050=0.50*$fac;
	$sf_025=0.25*$fac;

	$ym=$y1+(($y2-$y1)/3.0);

	fxpdf_linestyle($pdf, 0.0, 0.0, -$sf_050);

	if($left)
	{
		$nx1=$x1;
		$nx2=$x1+$sf_250;
	}
	else
	{
		$nx1=$x2-$sf_250;
		$nx2=$x2;
	}

	fxpdf_color($pdf, 'S', 0, '777777');
	$pdf->MoveTo($nx1+$sf_025, $ym-$sf_275);
	$pdf->LineTo($nx2-$sf_025, $ym-$sf_275, 'D');

	fxpdf_color($pdf, 'S', 0, '444444');
	$pdf->MoveTo($nx2, $ym-$sf_300);
	$pdf->LineTo($nx2, $ym-$sf_075, 'D');
	$pdf->MoveTo($nx1+$sf_025, $ym-$sf_100);
	$pdf->LineTo($nx2-$sf_025, $ym-$sf_100, 'D');

	fxpdf_color($pdf, 'S', 0, '777777');
	$pdf->MoveTo($nx1, $ym-$sf_300);
	$pdf->LineTo($nx1, $ym-$sf_075, 'D');
}

/**
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  FXPDF_PST_DEP (Abhängigkeits-Linie)                                      +
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf - Mandatory parameter:                 ???PARAMETER???
 * @param various $x1   - Mandatory parameter:                 ???PARAMETER???
 * @param various $y1   - Mandatory parameter:                 ???PARAMETER???
 * @param various $x2   - Mandatory parameter:                 ???PARAMETER???
 * @param various $y2   - Mandatory parameter:                 ???PARAMETER???
 * @param various $z    - Mandatory parameter:                 ???PARAMETER???
 * @param various $s    - Mandatory parameter:                 ???PARAMETER???
 * @param double  $fac  - Optional parameter (default = 0.0):  ???PARAMETER???
 */
function fxpdf_pst_dep(&$pdf, $x1, $y1, $x2, $y2, $z, $s, $fac=0.0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//echo('<b class="white">fxpdf_pst_dep(&$pdf, $x1='.$x1.', $y1='.$y1.', $x2='.$x2.', $y2='.$y2.', $z='.$z.', $s='.$s.')</b><br>');
	$ofac=$fac;
	if($fac < 0.0)
		$fac=-$fac;
	else if($fac == 0.0)
		$fac=$GLOBALS['pdf_array']['fac'];
	else
		$fac *= $GLOBALS['pdf_array']['fac'];

	$sf_050=0.50*$fac;
	$sf_025=0.25*$fac;

	$spid=(int)$GLOBALS['pscal'][$z][0]['pid'];
	if($GLOBALS['pda'][$spid] && fxIsArray($GLOBALS['pscal'][$z][0]['dep']))
	{
		$mw=$GLOBALS['pdf_array']['cwidth']/2.0;
		$mh=$GLOBALS['pdf_array']['cheight']/2.0;

		foreach($GLOBALS['pscal'][$z][0]['dep'] as $d)
		{
			$ls=$z;
			$cs=$d['col'];
			$ld=-1;
			$cd=-1;
			foreach($GLOBALS['pscal'] as $pl => $pd)
			{
				if(fxIsArray($pd) && fxIsArray($pd[0]) && isset($pd[0]['pid']))
				{
					$pid=(int)$pd[0]['pid'];
					if(($pid == $d['projekt_b']) && $GLOBALS['pda'][$pid])
					{
//fxDebug($pd,'$pd: $pl='.$pl);
						$ld=$pl;
//fxDebug($pid, '$ls='.$ls.', $ld='.$ld.' - $z='.$z.', $pl='.$pl);
						if($ld > $ls)
						{
							for($l=$ls+1; $l<$pl; $l++)
							{
								$lpid=(int)$GLOBALS['pscal'][$l][0]['pid'];
								if(!$GLOBALS['pda'][$lpid])
									$ld--;
							}
						}
						else if($ld < $ls)
						{
							for($l=$ls-1; $l>$pl; $l--)
							{
								$lpid=(int)$GLOBALS['pscal'][$l][0]['pid'];
								if(!$GLOBALS['pda'][$lpid])
									$ld++;
							}
						}
						if($ld > -1)
						{
							if(substr((string)$d['abh_typ'],1,1) == 'F')
								$cd=$pd[0]['ek'];
							else
								$cd=$pd[0]['sk'];
						}
						break;
					}
				}
			}
//fxDebug($d,'$d');
			if(($ld > -1) && ($cd > -1))
			{
				$ad='right';

				$ys=$y1+($y2-$y1)/3.6;
				$yd=$ys+($ld-$ls)*$GLOBALS['pdf_array']['cheight'];
//function fxpdf_pst_dep(&$pdf, $x2, $prx2, $y1, $py1, $pym, $z, $s)
//echo('<font class="white">'.$d['nr_a'].' ('.$d['abh_typ'].') '.$d['nr_b'].': $ls='.$ls.', $cs='.$cs.' - $ld='.$ld.', $cd='.$cd.'<br>$ys='.$ys.', $yd='.$yd.'</font><hr>');
				fxpdf_color($pdf, 'S', 0, '888888');
				fxpdf_linestyle($pdf, 0.0, 0.0, -$sf_025);
				if(substr((string)$d['abh_typ'],0,1) == 'S')
				{
					$ym=0;
					$xs=$x1;
					$xd=$xs+($cd-$cs)*$GLOBALS['pdf_array']['cwidth'];
					if(substr((string)$d['abh_typ'],1,1) == 'F')
					{
						$ad='left';
						$xd += $GLOBALS['pdf_array']['cwidth'];
						$ym=$yd-$GLOBALS['pdf_array']['cheight']/3.0;
					}

					$pdf->MoveTo($xs+$sf_050, $ys);
					$pdf->LineTo($xs-$mw, $ys);
					if($ym)
					{
						$pdf->LineTo($xs-$mw, $ym);
						$pdf->LineTo($xd+$mw, $ym);
						$pdf->LineTo($xd+$mw, $yd);
					}
					else
						$pdf->LineTo($xs-$mw, $yd);
					$pdf->LineTo($xd, $yd, 'D');

					// Pfeil zeichnen
					fxpdf_pst_arrow($pdf, $xd, $yd, $GLOBALS['pdf_array']['cwidth'], $ofac, $ad);
				}
				else
				{
					$ym=0;
					$xs=$x2;
					$xd=$x1+($cd-$cs)*$GLOBALS['pdf_array']['cwidth'];
					if(substr((string)$d['abh_typ'],1,1) == 'F')
					{
						$ad='left';
						$xd += $GLOBALS['pdf_array']['cwidth'];
					}
					else if($xd-$mw < $xs+$mw)
						$ym=$yd-$GLOBALS['pdf_array']['cheight']/3.0;

					$pdf->MoveTo($xs-$sf_050, $ys);
					if(substr((string)$d['abh_typ'],1,1) == 'F')
					{
						$pdf->LineTo($xd+$mw, $ys);
						$pdf->LineTo($xd+$mw, $yd);
					}
					else if($ym)
					{
						$pdf->LineTo($xs+$mw, $ys);
						$pdf->LineTo($xs+$mw, $ym);
						$pdf->LineTo($xd-$mw, $ym);
						$pdf->LineTo($xd-$mw, $yd);
					}
					else
					{
						$pdf->LineTo($xs+$mw, $ys);
						$pdf->LineTo($xs+$mw, $yd);
					}
					$pdf->LineTo($xd, $yd, 'D');

					// Pfeil zeichnen
					fxpdf_pst_arrow($pdf, $xd, $yd, $GLOBALS['pdf_array']['cwidth'], $ofac, $ad);
				}
			}
		}
	}
}

/**
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  FXPDF_PST_MST_DRAW (Meilensteine zeichnen)                                +
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf - Mandatory parameter:                 ???PARAMETER???
 * @param various $sx   - Mandatory parameter:                 ???PARAMETER???
 * @param various $ul   - Mandatory parameter:                 ???PARAMETER???
 * @param various $ur   - Mandatory parameter:                 ???PARAMETER???
 * @param various $sy   - Mandatory parameter:                 ???PARAMETER???
 * @param various $uy   - Mandatory parameter:                 ???PARAMETER???
 * @param double  $fac  - Optional parameter (default = 0.0):  ???PARAMETER???
 * @param various $type - Optional parameter (default = -1):   ???PARAMETER???
 */
function fxpdf_pst_mst_draw(&$pdf, $sx, $ul, $ur, $sy, $uy, $fac=0.0, $type=-1)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($fac < 0.0)
		$fac=-$fac;
	else if($fac == 0.0)
		$fac=$GLOBALS['pdf_array']['fac'];
	else
		$fac *= $GLOBALS['pdf_array']['fac'];

	$sf_050=0.50*$fac;

	fxpdf_linestyle($pdf, 0.0, 0.0, -$sf_050);

	$mca=array('de7f00', 'd09300', 'e6be63');
	if(!$type)	// fix
		$mca=array('ff7e00', '724814', '987300');

	fxpdf_color($pdf, 'F', 0, $mca[0]);	// Grau
	fxpdf_color($pdf, 'S', 0, $mca[1]);	// mit dunkelgrauem Rand

	$my=$sy+($uy-$sy)/2.0;

	$pdf->MoveTo($sx, $sy);
	$pdf->LineTo($ul, $my);
	$pdf->LineTo($sx, $uy);
	$pdf->LineTo($ur, $my, 'CDF');

	// Hellgrau
	fxpdf_color($pdf, 'S', 0, $mca[2]);

	$pdf->MoveTo($sx, $sy);
	$pdf->LineTo($ul, $my);
	$pdf->LineTo($sx, $uy, 'D');
}

/**
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  FXPDF_PST_ARROW (Pfeilspitze)                                             +
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf - Mandatory parameter:                     ???PARAMETER???
 * @param various $abxd - Mandatory parameter:                     ???PARAMETER???
 * @param various $abyd - Mandatory parameter:                     ???PARAMETER???
 * @param various $cw   - Mandatory parameter:                     ???PARAMETER???
 * @param double  $fac  - Optional parameter (default = 0.0):      ???PARAMETER???
 * @param string  $d    - Optional parameter (default = 'right'):  ???PARAMETER???
 */
function fxpdf_pst_arrow(&$pdf, $abxd, $abyd, $cw, $fac=0.0, $d='right')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($fac < 0.0)
		$fac=-$fac;
	else if($fac == 0.0)
		$fac=$GLOBALS['pdf_array']['fac'];
	else
		$fac *= $GLOBALS['pdf_array']['fac'];

	$sf_050=0.50*$fac;
	$sf_025=0.25*$fac;

	$d=strtolower(substr((string)$d,0,1));

	// Pfeil zeichnen
	$pd		= $cw*0.33;
	$pd2	= $pd/2.0;

	fxpdf_linestyle($pdf, 0.0, 0.0, -$sf_025);
	fxpdf_color($pdf, 'F', 0, 'aaaaaa');	// Hellgrau
	fxpdf_color($pdf, 'S', 0, '888888');	// mit dunkelgrauem Rand

	if($d == 'r')	// Right
	{
		$pdf->MoveTo($abxd+$sf_050, $abyd);
		$pdf->LineTo($abxd-$pd, $abyd+$pd2);
		$pdf->LineTo($abxd-$pd, $abyd-$pd2, 'CDF');
	}
	else			// Left
	{
		$pdf->MoveTo($abxd-$sf_050, $abyd);
		$pdf->LineTo($abxd+$pd, $abyd+$pd2);
		$pdf->LineTo($abxd+$pd, $abyd-$pd2, 'CDF');
	}
}
?>