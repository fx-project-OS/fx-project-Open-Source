<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : tools_pdf.inc                                                //
// Version     : 21.1                                                         //
// Begin       : 2020-11-19                                                   //
// Last Change : 2020-11-19                                                   //
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
 * PDF function collection
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;

define('FXPDF_NONE',	0);
define('FXPDF_FPDF',	1);
define('FXPDF_TCPDF',	2);


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * Get the currently used PDF mode
 *
 * @return Currently used PDF mode (0=None, 1=FPDF or 2=TCPDF)
 */
function fxpdf_mode()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(isset($GLOBALS['_ext_pdf']))
	{
		if($GLOBALS['_ext_pdf'] == 'TCPDF')
			return FXPDF_TCPDF;
		if($GLOBALS['_ext_pdf'] == 'FPDF')
			return FXPDF_FPDF;
	}
	return FXPDF_NONE;
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * + FXPDF_PAGE_DIMENSIONS														+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param boolean $pwa - Optional parameter (default=false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fxpdf_page_dimensions($pwa=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(fxIsArray($pwa) && (sizeof($pwa) > 1))
	{
		$pw=$pwa[0];
		$ph=$pwa[1];
	}
	else
	{
		$pw=$GLOBALS['pdf_array']['width'];
		$ph=$GLOBALS['pdf_array']['height'];
	}
	$darr=array('P', $pw, $ph);
	if($pw > $ph)	// Querformat
	{
		$darr[0]='L';
		if(fxpdf_mode() == FXPDF_TCPDF)
		{
			$darr[1]=$ph;
			$darr[2]=$pw;
		}
	}

	return $darr;
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * + FXPDF_NEW																	+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @return ???RETURN???
 */
function fxpdf_new()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$darr=fxpdf_page_dimensions();
	if(fxpdf_mode() == FXPDF_TCPDF)
	{
		$pdf=new xTCPDF($darr[0], 'pt', array($darr[1], $darr[2]), false, FXP_CHARSET, false);
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;border:2px solid #006b9f;margin:3px 0;padding:3px;">$pdf = new <b>TCPDF</b>('.$darr[0].', \'pt\', array('.$darr[1].', '.$darr[2].', false, FXP_CHARSET, false));</div>'.$GLOBALS['nl']);
	}
	else
	{
		$pdf=new xFPDF($darr[0], 'pt', array($darr[1], $darr[2]));
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;border:2px solid #006b9f;margin:3px 0;padding:3px;">$pdf = new <b>FPDF</b>('.$darr[0].', \'pt\', array('.$darr[1].', '.$darr[2].'));</div>'.$GLOBALS['nl']);
	}

	return $pdf;
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * + FXPDF_PAGE																	+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf - Mandatory parameter:                    ???PARAMETER???
 * @param string  $mode - Optional parameter (default='open'):  ???PARAMETER???
 * @param boolean $pwh  - Optional parameter (default=false):   ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fxpdf_page(&$pdf, $mode='open', $pwh=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$rv=false;
	$mode=strtolower(substr($mode, 0,1));

	switch($mode)
	{
		case 'o':	// Open New Page
			// ...Set document information
			$title='fx-project: PDF';
			if($GLOBALS['pdf_array']['i_title'])
				$title=$GLOBALS['pdf_array']['i_title'];
			$subject='fx-project: Document';
			if($GLOBALS['pdf_array']['i_subject'])
				$subject=$GLOBALS['pdf_array']['i_subject'];

			$pdf->SetCreator('fx-project');
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>SetCreator</b>(\'fx-project\');</div>'.$GLOBALS['nl']);
			if(isset($GLOBALS['author_string']) && strlen($GLOBALS['author_string']))
			{
				$pdf->SetAuthor($GLOBALS['author_string']);
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>SetAuthor</b>(\''.$GLOBALS['author_string'].'\');</div>'.$GLOBALS['nl']);
			}
			$pdf->SetTitle($title);
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>SetTitle</b>(\'fx-project\');</div>'.$GLOBALS['nl']);
			$pdf->SetSubject($subject);
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>SetSubject</b>(\''.$subject.'\');</div>'.$GLOBALS['nl']);
			$pdf->SetKeywords('fx-project, PDF, '.$title);
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>SetKeywords</b>(\'fx-project, PDF, '.$title.'\');</div>'.$GLOBALS['nl']);

			// ...Disable auto page breaks
			$pdf->SetAutoPageBreak(false);
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>SetAutoPageBreak</b>(false);</div>'.$GLOBALS['nl']);

			if(fxpdf_mode() == FXPDF_TCPDF)
			{
				$pdf->SetDisplayMode('real', 'OneColumn', 'UseNone');
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>SetDisplayMode</b>(\'real\', \'OneColumn\', \'UseNone\');</div>'.$GLOBALS['nl']);

				// ...Disable header and footer
				$pdf->setPrintHeader(false);
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>setPrintHeader</b>(false);</div>'.$GLOBALS['nl']);
				$pdf->setPrintFooter(false);
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>setPrintFooter</b>(false);</div>'.$GLOBALS['nl']);

				// ...Set default monospaced font
				$pdf->SetDefaultMonospacedFont(PDF_FONT_MONOSPACED);
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>SetDefaultMonospacedFont</b>('.PDF_FONT_MONOSPACED.');</div>'.$GLOBALS['nl']);

				// ...Set image scale factor
				if(isset($GLOBALS['pdf_picscale']))
				{
					$pdf->setImageScale($GLOBALS['pdf_picscale']); 
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>setImageScale</b>('.$GLOBALS['pdf_picscale'].');</div>'.$GLOBALS['nl']);
				}
				else
				{
					$pdf->setImageScale(PDF_IMAGE_SCALE_RATIO); 
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>setImageScale</b>('.PDF_IMAGE_SCALE_RATIO.');</div>'.$GLOBALS['nl']);
				}

				// ...Set some language-dependent strings
				$pdf->setLanguageArray($GLOBALS['l']);
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>setLanguageArray</b>('.$GLOBALS['l'].');</div>'.$GLOBALS['nl']);
			}
			else
			{
				// Alias aktivieren für Gesamtseitenzahl
				$pdf->AliasNbPages();
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>AliasNbPages</b>();</div>'.$GLOBALS['nl']);
			}

			//...Set margins
			$pdf->SetMargins($GLOBALS['pdf_array']['lmargin'], $GLOBALS['pdf_array']['tmargin'], $GLOBALS['pdf_array']['rmargin']);
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>SetMargins</b>('.$GLOBALS['pdf_array']['lmargin'].', '.$GLOBALS['pdf_array']['tmargin'].', '.$GLOBALS['pdf_array']['rmargin'].');</div>'.$GLOBALS['nl']);
		break;

		case 'b':	// Beginn Page
			$darr=fxpdf_page_dimensions($pwh);
			$pdf->AddPage($darr[0], array($darr[1], $darr[2]));
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>AddPage</b>('.$darr[0].', array('.$darr[1].', '.$darr[2].'));</div>'.$GLOBALS['nl']);
			fxpdf_linestyle($pdf);
		break;

		case 'e':	// End Page
			// Wird nicht benötigt hier
		break;

		case 'c':	// Close Page
			// Daten in String zwischenspeichern
			if(fxpdf_mode() == FXPDF_TCPDF)
			{
				$rv=$pdf->Output('dummy', 'S');
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$rv = $pdf-><b>Output</b>(\'dummy\', \'S\');</div>'.$GLOBALS['nl']);
			}
			else
			{
				$rv=$pdf->Output('S');
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$rv = $pdf-><b>Output</b>(\'S\');</div>'.$GLOBALS['nl']);
			}
		break;
	}

	return $rv;
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * + FXPDF_LINESTYLE															+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf - Mandatory parameter:                  ???PARAMETER???
 * @param double  $wd   - Optional parameter (default=0.0):   ???PARAMETER???
 * @param double  $bd   - Optional parameter (default=0.0):   ???PARAMETER???
 * @param double  $lw   - Optional parameter (default=0.35):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fxpdf_linestyle(&$pdf, $wd=0.0, $bd=0.0, $lw=0.35)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($lw > 0.0)
		$lw *= $GLOBALS['pdf_array']['fac'];
	else if($lw < 0.0)
		$lw=-$lw;
	else
		$lw=0.35*$GLOBALS['pdf_array']['fac'];

	if(fxpdf_mode() == FXPDF_TCPDF)
	{
		$style_array=array(
			'width'	=>	$lw,
			'cap'	=> "square",
			'join'	=> "round",
			'dash'	=> ""
		);

		if(($wd > 0.0) || ($bd > 0.0))
			$style_array['dash']=($wd*$GLOBALS['pdf_array']['fac']).','.($bd*$GLOBALS['pdf_array']['fac']);

		$pdf->SetLineStyle($style_array);
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>SetLineStyle</b>(array(\'width\'=>'.$style_array['width'].', \'cpa\'=>\''.$style_array['cap'].'\', \'join\'=>\''.$style_array['join'].'\', \'dash\'=>\''.$style_array['dash'].'\'));</div>'.$GLOBALS['nl']);
	}
	else
	{
		$pdf->SetLineWidth($lw);
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>SetLineWidth</b>('.$lw.');</div>'.$GLOBALS['nl']);
		if(($wd > 0.0) || ($bd > 0.0))
		{
			$pdf->SetLineDash($wd*$GLOBALS['pdf_array']['fac'], $bd*$GLOBALS['pdf_array']['fac']);
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>SetLineDash</b>('.($wd*$GLOBALS['pdf_array']['fac']).', '.($bd*$GLOBALS['pdf_array']['fac']).');</div>'.$GLOBALS['nl']);
		}
		else
		{
			$pdf->SetLineDash();
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>SetLineDash</b>();</div>'.$GLOBALS['nl']);
		}
	}

	return $lw;
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * + FXPDF_TEXT																	+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf  - Mandatory parameter:                    ???PARAMETER???
 * @param various $text  - Mandatory parameter:                    ???PARAMETER???
 * @param various $xc    - Mandatory parameter:                    ???PARAMETER???
 * @param various $yc    - Mandatory parameter:                    ???PARAMETER???
 * @param double  $sf    - Optional parameter (default=8.0):       ???PARAMETER???
 * @param string  $align - Optional parameter (default='left'):    ???PARAMETER???
 * @param string  $mode  - Optional parameter (default='normal'):  ???PARAMETER???
 * @param double  $lw    - Optional parameter (default=0.25):      ???PARAMETER???
 * @param string  $rgb   - Optional parameter (default='000000'):  ???PARAMETER???
 * @param integer $deg   - Optional parameter (default=0):         ???PARAMETER???
 * @param double  $space - Optional parameter (default=0.0):       ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fxpdf_text(&$pdf, $text, $xc, $yc, $sf=8.0, $align='left', $mode='normal', $lw=0.25, $rgb='000000', $deg=0, $space=0.0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$text=fxpdf_convert($text);

	$xa		= fxpdf_getx($xc);
	$ya		= fxpdf_gety($yc);
	$align	= substr(strtolower($align), 0, 3);
	$mode	= substr(strtolower($mode), 0, 3);
	$tarr	= explode('<|>', $text);
	$rw		= 0.0;
	$space	= $space*$GLOBALS['pdf_array']['fac'];

	// Font setzen
	if(($mode == 'bol') || ($mode == 'out') || ($mode == 'sha') || ($mode == 'sta') || ($mode == 'ste'))
		fxpdf_font($pdf, $sf, 'B');	// Bold
	else if($mode == 'ita')
		fxpdf_font($pdf, $sf, 'I');	// Italic
	else if($mode == 'und')
		fxpdf_font($pdf, $sf, 'U');	// Underline
	else if($mode == 'nor')
		fxpdf_font($pdf, $sf);		// Normal
	else
		fxpdf_font($pdf, $sf, $mode);

	foreach($tarr as $ln => $text)
	{
		$s_width=$pdf->GetStringWidth($text);
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$s_width = $pdf-><b>GetStringWidth</b>(\''.$text.'\'); &rarr; '.$s_width.'</div>'.$GLOBALS['nl']);
		$s_height=$sf;
		if(fxpdf_mode() == FXPDF_TCPDF)
		{
			$s_height=$pdf->GetFontSize();
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$s_height = $pdf-><b>GetFontSize</b>(); &rarr; '.$s_height.'</div>'.$GLOBALS['nl']);
		}
		$rw=max($s_width, $rw);

		if($deg > 0.0)
		{
			$yc=$ya;
			$xc=$xa + $ln*($s_height+$space);
			// Y-Koordinate nach links verschieben, da Textfunktion von TCPDF nicht genau die übermittelte Y-Koordinate verwendet?
			if(fxpdf_mode() == FXPDF_TCPDF)
				$yc += 2.80;
			switch($align)
			{
				case 'rig': 	// Rechtsbündig
					$yc=$yc+$s_width;
					break;

				case 'cen': 	// Zentriert
					$yc=$yc+($s_width/2);
					break;
			}
		}
		else
		{
			$xc=$xa;
			$yc=$ya + $ln*($s_height+$space);
			// X-Koordinate nach links verschieben, da Textfunktion von TCPDF nicht genau die übermittelte X-Koordinate verwendet?
			if(fxpdf_mode() == FXPDF_TCPDF)
				$xc -= 2.80;
			switch($align)
			{
				case 'rig': 	// Rechtsbündig
					$xc=$xc-$s_width;
					break;

				case 'cen': 	// Zentriert
					$xc=$xc-($s_width/2);
					break;
			}
		}

		if($deg > 0.0)
		{
			if(fxpdf_mode() == FXPDF_TCPDF)
			{
				$pdf->StartTransform();
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>StartTransform</b>();</div>'.$GLOBALS['nl']);
			}
			$pdf->Rotate($deg, $xc, $yc);
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>Rotate</b>('.$deg.', '.$xc.', '.$yc.');</div>'.$GLOBALS['nl']);
		}

		$lw=fxpdf_linestyle($pdf, 0.0, 0.0, $lw);
		switch($mode)
		{
			case 'out':		// OUTLINE: Umranded
				fxpdf_color($pdf, 'T', 0, 'ffffff');
				fxpdf_color($pdf, 'S', 0, $rgb);
				if(fxpdf_mode() == FXPDF_TCPDF)
				{
					$pdf->Text($xc, $yc, $text, false, false, true, 0, 0, 'L', 0, '', 0, false, 'L', 'M');
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>Text</b>('.$xc.', '.$yc.', \''.$text.'\', false, false, true, 0, 0, \'L\', 0, \'\', 0, false, \'L\', \'M\');</div>'.$GLOBALS['nl']);
				}
				else
				{
					$pdf->Text($xc, $yc, $text, 2);
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>Text</b>('.$xc.', '.$yc.', \''.$text.'\', 2);</div>'.$GLOBALS['nl']);
				}
			break;

			case 'sha':		// SHADOW: Schatten
				$lws=$lw*2.50;
				fxpdf_color($pdf, 'TS', 0, '808080');
				if(fxpdf_mode() == FXPDF_TCPDF)
				{
					$pdf->Text($xc+$lws, $yc+$lws, $text, false, false, true, 0, 0, 'L', 0, '', 0, false, 'L', 'M');
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>Text</b>('.($xc+$lws).', '.($yc+$lws).', \''.$text.'\', false, false, true, 0, 0, \'L\', 0, \'\', 0, false, \'L\', \'M\');</div>'.$GLOBALS['nl']);
				}
				else
				{
					$pdf->Text($xc+$lws, $yc+$lws, $text, 2);
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>Text</b>('.($xc+$lws).', '.($yc+$lws).', \''.$text.'\', 2);</div>'.$GLOBALS['nl']);
				}
				fxpdf_color($pdf, 'T', 0, 'ffffff');
				fxpdf_color($pdf, 'S', 0, $rgb);
				if(fxpdf_mode() == FXPDF_TCPDF)
				{
					$pdf->Text($xc, $yc, $text, false, false, true, 0, 0, 'L', 0, '', 0, false, 'L', 'M');
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>Text</b>('.$xc.', '.$yc.', \''.$text.'\', false, false, true, 0, 0, \'L\', 0, \'\', 0, false, \'L\', \'M\');</div>'.$GLOBALS['nl']);
				}
				else
				{
					$pdf->Text($xc, $yc, $text, 2);
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>Text</b>('.$xc.', '.$yc.', \''.$text.'\', 2);</div>'.$GLOBALS['nl']);
				}
			break;

			case 'sta':		// STAMP: Stempel
			case 'ste':
				$lws=$lw*2.50;
				fxpdf_color($pdf, 'TS', 0, '808080');
				if(fxpdf_mode() == FXPDF_TCPDF)
				{
					$pdf->Text($xc-$lws, $yc-$lws, $text, false, false, true, 0, 0, 'L', 0, '', 0, false, 'L', 'M');
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>Text</b>('.($xc-$lws).', '.($yc-$lws).', \''.$text.'\', false, false, true, 0, 0, \'L\', 0, \'\', 0, false, \'L\', \'M\');</div>'.$GLOBALS['nl']);
				}
				else
				{
					$pdf->Text($xc-$lws, $yc-$lws, $text, 2);
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>Text</b>('.($xc-$lws).', '.($yc-$lws).', \''.$text.'\', 2);</div>'.$GLOBALS['nl']);
				}
				fxpdf_color($pdf, 'T', 0, 'ffffff');
				fxpdf_color($pdf, 'S', 0, $rgb);
				if(fxpdf_mode() == FXPDF_TCPDF)
				{
					$pdf->Text($xc, $yc, $text, false, false, true, 0, 0, 'L', 0, '', 0, false, 'L', 'M');
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>Text</b>('.$xc.', '.$yc.', \''.$text.'\', false, false, true, 0, 0, \'L\', 0, \'\', 0, false, \'L\', \'M\');</div>'.$GLOBALS['nl']);
				}
				else
				{
					$pdf->Text($xc, $yc, $text, 2);
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>Text</b>('.$xc.', '.$yc.', \''.$text.'\', 2);</div>'.$GLOBALS['nl']);
				}
			break;

			default:			// Normal/Bold/Italic/Underline
				fxpdf_color($pdf, 'T', 0, $rgb);
				if(fxpdf_mode() == FXPDF_TCPDF)
				{
					$pdf->Text($xc, $yc, $text, false, false, true, 0, 0, 'L', false, '', 0, false, 'L', 'M');
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>Text</b>('.$xc.', '.$yc.', \''.$text.'\', false, false, true, 0, 0, \'L\', false, \'\', 0, false, \'L\', \'M\');</div>'.$GLOBALS['nl']);
				}
				else
				{
					$pdf->Text($xc, $yc, $text);
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>Text</b>('.$xc.', '.$yc.', \''.$text.'\');</div>'.$GLOBALS['nl']);
				}
			break;
		}

		if($deg > 0.0)
		{
			if(fxpdf_mode() == FXPDF_TCPDF)
			{
				$pdf->StopTransform();
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>StopTransform</b>();</div>'.$GLOBALS['nl']);
			}
			$pdf->Rotate(0);
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$pdf-><b>Rotate</b>(0);</div>'.$GLOBALS['nl']);
		}
	}

	return $rw;
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  FXPDF_PICSIZE																+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various $pic - Mandatory parameter:                 ???PARAMETER???
 * @param double  $sf  - Optional parameter (default=1.0):  ???PARAMETER???
 * @param string  $pw  - Optional parameter (default=''):   ???PARAMETER???
 * @param string  $ph  - Optional parameter (default=''):   ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fxpdf_picsize($pic, $sf=1.0, $pw='', $ph='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$size=array('owidth'=>0, 'oheight'=>0, 'width'=>0, 'height'=>0, 'suffix'=>'', 'path'=>'', 'fac'=>$sf);

	if(!$pic || ($pic == 'dmy.png') || ($pic == $GLOBALS['gfxpath'].'dmy.png'))
		return $size;

	// Dummybild ersetzen durch Mandanten- bzw. FeRox-Logo
	$autoscale=false;
	if(strtolower($pic) == 'fx-project')
	{
		$pic='';
		$psa=array('png', 'jpg', 'gif');
		$lsa=array($GLOBALS['datpath'].$GLOBALS['fxpglobals']['client'].'/logo.', $GLOBALS['datpath'].'0/logo.');
		foreach($lsa as $ls)
		{
			foreach($psa as $ps)
			{
				if(file_exists($ls.$ps))
				{
					$pic=$ls.$ps;
					$autoscale=true;
					break 2;
				}
			}
		}

		if(!strlen($pic))
		{
			$pic=$GLOBALS['datpath'].'0/logo.png';
			$png=base64_decode(substr(fxGetLogo('medium'),22));
			fxSaveBin($pic, $png);
			$autoscale=true;
		}
//echo('Dummylogo ersetzt durch: $pic='.$pic.'<br />');
	}

	// Pfad extrahieren
	$prefix='';
	if(mb_substr($pic,0,strlen($GLOBALS['gfxpath'])) == $GLOBALS['gfxpath'])
	{
		$pic=mb_substr($pic,strlen($GLOBALS['gfxpath']));
		if(mb_substr($GLOBALS['gfxpath'], 0, 2) == './')
			$prefix=mb_substr($GLOBALS['gfxpath'],2);
		else
			$prefix=$GLOBALS['gfxpath'];
	}
	$pic=trim($pic);
	if(mb_substr($pic,0,2) == './')
		$pic=mb_substr($pic, 2);

	// Parameter?
	$qp=strpos($pic,'?');
	if($qp !== false)
		$pic=trim(mb_substr($pic,0,$qp));

	// Pfad ermitteln
	$pp=$GLOBALS['datpath'].$GLOBALS['fxpglobals']['client'].'/'.$pic;
	if(!file_exists($pp))
		$pp=$GLOBALS['datpath'].'0/'.$pic;
	if(!file_exists($pp))
		$pp=$prefix.$pic;
	$size['path']=$pp;
	if(file_exists($pp))
	{
		// Prüfen, ob Grafik im richtigen Format
		// - 0: Breite
		// - 1: Höhe
		// - 2: Grafiktyp (IMAGETYPE_...): 1=GIF, 2=JPG, 3=PNG, 4=SWF, 5=PSD, 6=BMP, 7=TIFF(intel byte order), 8=TIFF(motorola byte order), 9=JPC, 10=JP2, 11=JPX, 12=JB2, 13=SWC, 14=IFF, 15=WBMP, 16=XBM
		$ims=GetImageSize($pp);
//fxDebug($ims, $pp);
		if(is_array($ims))
		{
			if($ims[2] == 1)		// GIF
				$size['suffix']='gif';
			else if($ims[2] == 2)	// JPG
				$size['suffix']='jpg';
			else if($ims[2] == 3)	// PNG
				$size['suffix']='png';

			if($size['suffix'])
			{
				$size['owidth']=$ims[0];
				$size['oheight']=$ims[1];

				// Faktor ermitteln
				if($autoscale)
				{
					$hfac=round((140.0*$GLOBALS['pdf_picscale'])/$ims[1],2);
					$wfac=$hfac;
				}
				else
				{
					$wfac=1.0;
					if($pw && ($pw != $ims[0]))
						$wfac=$pw/$ims[0];
					if($wfac == 1.0)
						$wfac=$sf;
					$wfac *= $GLOBALS['pdf_picscale'];

					$hfac=1.0;
					if($ph && ($ph != $ims[1]))
						$hfac=$ph/$ims[1];
					if($hfac == 1.0)
						$hfac=$sf;
					$hfac *= $GLOBALS['pdf_picscale'];
				}

				// Max. Seitenhöhe berücksichtigen
				if(!is_double($ph))
				{
					$hb  = $GLOBALS['pdf_array']['height']-$GLOBALS['pdf_array']['tmargin']-$GLOBALS['pdf_array']['h_pfoot']-$GLOBALS['pdf_array']['bmargin'];
					$hb -= ($GLOBALS['pdf_array']['fshl'] + $GLOBALS['pdf_array']['fsh'] + 12.0*$GLOBALS['pdf_array']['yd']);
					$hb /= 0.80;

					if($ims[1]*$hfac > $hb)
					{
						$oldf=$hfac;
	 					$hfac=$hb/$ims[1];
						$wfac=$hfac*$wfac/$oldf;
//echo('--&gt; $oldf='.$oldf.', <b>$hfac='.$hfac.', $wfac='.$wfac.'</b><br>');
					}
				}

				$fac=min($wfac,$hfac);

				if(!$pw || !is_double($pw))
					$size['width']=$ims[0]*$wfac;
				else
					$size['width']=$pw;

				if(!$ph || !is_double($ph))
					$size['height']=$ims[1]*$hfac;
				else
					$size['height']=$ph;

				$size['wfac']=$wfac;
				$size['hfac']=$hfac;
				$size['fac']=$fac;
			}
		}
	}
//fxDebug($size,'$size', 0);

	return $size;
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * + FXPDF_PICTURE																+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf  - Mandatory parameter:                    ???PARAMETER???
 * @param various $pic   - Mandatory parameter:                    ???PARAMETER???
 * @param various $xc    - Mandatory parameter:                    ???PARAMETER???
 * @param various $yc    - Mandatory parameter:                    ???PARAMETER???
 * @param double  $sf    - Optional parameter (default=1.0):     ???PARAMETER???
 * @param string  $align - Optional parameter (default='left'):  ???PARAMETER???
 * @param string  $pw    - Optional parameter (default=''):      ???PARAMETER???
 * @param string  $ph    - Optional parameter (default=''):      ???PARAMETER???
 * @param string  $frgb  - Optional parameter (default=''):      ???PARAMETER???
 */
function fxpdf_picture(&$pdf, $pic, $xc, $yc, $sf=1.0, $align='left', $pw='', $ph='', $frgb='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$size=fxpdf_picsize($pic, $sf, $pw, $ph);
	if(!$size['width'])
		return;

	// Bisher werden nur 1=GIF, 2=JPG und 3=PNG unterstützt
	if(($size['suffix'] != 'gif') && ($size['suffix'] != 'jpg') && ($size['suffix'] != 'png'))
		return;

	$xc=fxpdf_getx($xc);
	$yc=fxpdf_gety($yc) + 2*$GLOBALS['pdf_array']['fac'];

	$al3=substr(strtolower($align),0,3);
	switch($al3)
	{
		case 'rig': // Rechtsbündig
			$xc=$xc-$size['width'];
		break;

		case 'cen': // Zentriert
			$xc=$xc-($size['width']/2);
		break;
	}

	// 3D-Rahmen
	if(strlen($frgb))
	{
		$lw=fxpdf_linestyle($pdf);

		// Oben+Link
		fxpdf_color($pdf, 'D', 48, $frgb);
		$pdf->Line($xc, $yc-$lw, $xc+$size['width'], $yc-$lw);
		$pdf->Line($xc-$lw, $yc, $xc-$lw, $yc+$size['height']);

		// Unten+Rechts
		fxpdf_color($pdf, 'D', 0, $frgb);
		$pdf->Line($xc, $yc+$size['height']+$lw, $xc+$size['width'], $yc+$size['height']+$lw);
		$pdf->Line($xc+$size['width']+$lw, $yc, $xc+$size['width']+$lw, $yc+$size['height']);
	}

	$pdf->Image($size['path'], $xc, $yc, $size['width'], $size['height'], $size['suffix']);
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * + FXPDF_REC																	+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf - Mandatory parameter:                      ???PARAMETER???
 * @param various $x1   - Mandatory parameter:                      ???PARAMETER???
 * @param various $y1   - Mandatory parameter:                      ???PARAMETER???
 * @param various $x2   - Mandatory parameter:                      ???PARAMETER???
 * @param various $y2   - Mandatory parameter:                      ???PARAMETER???
 * @param double  $wd   - Optional parameter (default=0.0):       ???PARAMETER???
 * @param double  $bd   - Optional parameter (default=0.0):       ???PARAMETER???
 * @param double  $lw   - Optional parameter (default=0.35):      ???PARAMETER???
 * @param string  $frgb - Optional parameter (default='bbbbbb'):  ???PARAMETER???
 */
function fxpdf_rec(&$pdf, $x1, $y1, $x2, $y2, $wd=0.0, $bd=0.0, $lw=0.35, $frgb='bbbbbb')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$x1=fxpdf_getx($x1);
	$x2=fxpdf_getx($x2);
	if($x1 > $x2)
	{
		$xr=$x1;
		$x1=$x2;
		$x2=$xr;
	}

	$y1=fxpdf_gety($y1);
	$y2=fxpdf_gety($y2);
	if($y1 > $y2)
	{
		$yr=$y1;
		$y1=$y2;
		$y2=$yr;
	}

	fxpdf_linestyle($pdf, $wd, $bd, $lw);
	fxpdf_color($pdf, 'D', 0, $frgb);
	$pdf->Rect($x1, $y1, $x2-$x1, $y2-$y1, 'D');
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * + FXPDF_BOX																	+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf - Mandatory parameter:                      ???PARAMETER???
 * @param various $x1   - Mandatory parameter:                      ???PARAMETER???
 * @param various $y1   - Mandatory parameter:                      ???PARAMETER???
 * @param various $x2   - Mandatory parameter:                      ???PARAMETER???
 * @param various $y2   - Mandatory parameter:                      ???PARAMETER???
 * @param string  $rgb  - Optional parameter (default='ffffff'):  ???PARAMETER???
 * @param string  $fb   - Optional parameter (default='1111'):    ???PARAMETER???
 * @param double  $wd   - Optional parameter (default=0.0):       ???PARAMETER???
 * @param double  $bd   - Optional parameter (default=0.0):       ???PARAMETER???
 * @param double  $lw   - Optional parameter (default=0.35):      ???PARAMETER???
 * @param string  $frgb - Optional parameter (default='bbbbbb'):  ???PARAMETER???
 */
function fxpdf_box(&$pdf, $x1, $y1, $x2, $y2, $rgb='ffffff', $fb='1111', $wd=0.0, $bd=0.0, $lw=0.35, $frgb='bbbbbb')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$x1=fxpdf_getx($x1);
	$x2=fxpdf_getx($x2);
	if($x1 > $x2)
	{
		$xr=$x1;
		$x1=$x2;
		$x2=$xr;
	}

	$y1=fxpdf_gety($y1);
	$y2=fxpdf_gety($y2);
	if($y1 > $y2)
	{
		$yr=$y1;
		$y1=$y2;
		$y2=$yr;
	}

	// Override: 3D-Rahmen
	if(mb_substr($fb, 0,1) == '#')
	{
		$inf=true;;
		if(strlen($fb) > 1)
			$fb=mb_substr($fb, 1);
		else
			$fb='1111';
	}
	else
		$inf=false;

	$lw=fxpdf_linestyle($pdf, $wd, $bd, $lw);
	fxpdf_color($pdf, 'F', 0, $rgb);

	// Box mit komplettem Rahmen
	if($fb == '1111')
	{
		fxpdf_color($pdf, 'D', 0, $frgb);
		$pdf->Rect($x1, $y1, $x2-$x1, $y2-$y1, 'DF');
	}

	// Box
	else
	{
		// Füllfarbe
		$pdf->Rect($x1, $y1, $x2-$x1, $y2-$y1, 'F');

		if(strpos($fb, '1') !== false)
			fxpdf_color($pdf, 'D', 0, $frgb);
		// Links
		if(mb_substr($fb, 0,1) == '1')
			$pdf->Line($x1, $y1, $x1, $y2);
		// Oben
		if(mb_substr($fb, 1,1) == '1')
			$pdf->Line($x1, $y1, $x2, $y1);
		// Rechts
		if(mb_substr($fb, 2,1) == '1')
			$pdf->Line($x2, $y1, $x2, $y2);
		// Unten
		if(mb_substr($fb, 3,1) == '1')
			$pdf->Line($x1, $y2, $x2, $y2);
	}

	// 3D-Rahmen
	if($inf)
	{
		// Links+Oben
		fxpdf_color($pdf, 'D', 64, $rgb);
		$pdf->Line($x1+$lw, $y2-$lw, $x1+$lw, $y1+$lw);
		$pdf->Line($x1+$lw, $y1+$lw, $x2-$lw, $y1+$lw);

		// Rechts+Unten
		fxpdf_color($pdf, 'D', -64, $rgb);
		$pdf->Line($x1+$lw, $y2-$lw, $x2-$lw, $y2-$lw);
		$pdf->Line($x2-$lw, $y2-$lw, $x2-$lw, $y1+$lw);
	}
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * + FXPDF_GBOX																	+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf - Mandatory parameter:                      ???PARAMETER???
 * @param various $x1   - Mandatory parameter:                      ???PARAMETER???
 * @param various $y1   - Mandatory parameter:                      ???PARAMETER???
 * @param various $x2   - Mandatory parameter:                      ???PARAMETER???
 * @param various $y2   - Mandatory parameter:                      ???PARAMETER???
 * @param string  $rgb1 - Optional parameter (default='ffffff'):  ???PARAMETER???
 * @param string  $rgb2 - Optional parameter (default='ffffff'):  ???PARAMETER???
 * @param boolean $bbox - Optional parameter (default=false):     ???PARAMETER???
 */
function fxpdf_gbox(&$pdf, $x1, $y1, $x2, $y2, $rgb1='ffffff', $rgb2='ffffff', $bbox=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$x1=fxpdf_getx($x1);
	$x2=fxpdf_getx($x2);
	if($x1 > $x2)
	{
		$xr=$x1;
		$x1=$x2;
		$x2=$xr;
	}

	$y1=fxpdf_gety($y1);
	$y2=fxpdf_gety($y2);
	if($y1 > $y2)
	{
		$yr=$y1;
		$y1=$y2;
		$y2=$yr;
	}

	if(($rgb1 == $rgb2) || (fxpdf_mode() != FXPDF_TCPDF))
		fxpdf_box($pdf, '&'.$x1, '&'.$y1, '&'.$x2, '&'.$y2, $rgb1, '0000');
	else
	{
		if(!$bbox || !is_array($bbox) || (sizeof($bbox) != 4))
			$bbox=array(0,0,1,0);
		$pdf->LinearGradient($x1, $y1, $x2-$x1, $y2-$y1, fxpdf_color($pdf, 'A', 0, $rgb1), fxpdf_color($pdf, 'A', 0, $rgb2), $bbox);
	}
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * + FXPDF_LINE																	+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf - Mandatory parameter:                      ???PARAMETER???
 * @param various $x1   - Mandatory parameter:                      ???PARAMETER???
 * @param various $y1   - Mandatory parameter:                      ???PARAMETER???
 * @param various $x2   - Mandatory parameter:                      ???PARAMETER???
 * @param various $y2   - Mandatory parameter:                      ???PARAMETER???
 * @param double  $wd   - Optional parameter (default=0.0):       ???PARAMETER???
 * @param double  $bd   - Optional parameter (default=0.0):       ???PARAMETER???
 * @param double  $lw   - Optional parameter (default=0.35):      ???PARAMETER???
 * @param string  $rgb  - Optional parameter (default='bbbbbb'):  ???PARAMETER???
 */
function fxpdf_line(&$pdf, $x1, $y1, $x2, $y2, $wd=0.0, $bd=0.0, $lw=0.35, $rgb='bbbbbb')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$x1=fxpdf_getx($x1);
	$x2=fxpdf_getx($x2);
	$y1=fxpdf_gety($y1);
	$y2=fxpdf_gety($y2);

	fxpdf_linestyle($pdf, $wd, $bd, $lw);
	fxpdf_color($pdf, 'D', 0, $rgb);
	$pdf->Line($x1, $y1, $x2, $y2);
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * + FXPDF_CLIPPING																+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf   - Mandatory parameter:                   ???PARAMETER???
 * @param various $mode   - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $hh     - Optional parameter (default=false):  ???PARAMETER???
 * @param boolean $hf     - Optional parameter (default=false):  ???PARAMETER???
 * @param string  $dmb    - Optional parameter (default=''):     ???PARAMETER???
 * @param boolean $cdebug - Optional parameter (default=false):  ???PARAMETER???
 */
function fxpdf_clipping(&$pdf, $mode, $hh=false, $hf=false, $dmb='', $cdebug=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$GLOBALS['pdf_array']['min_x']=$GLOBALS['pdf_array']['lmargin'];
	$GLOBALS['pdf_array']['max_x']=$GLOBALS['pdf_array']['width']-$GLOBALS['pdf_array']['rmargin'];
	switch($mode)
	{
		case 1:	// Kopf oder Fuss
			if($hh)	// Kopf
			{
				$GLOBALS['pdf_array']['min_y']	= $GLOBALS['pdf_array']['tmargin'];
				$GLOBALS['pdf_array']['max_y']	= $GLOBALS['pdf_array']['min_y']+$GLOBALS['pdf_array']['h_head'];
				$rh='h_head';
			}
			else		// Fuss
			{
				$GLOBALS['pdf_array']['min_y']	= $GLOBALS['pdf_array']['height']-$GLOBALS['pdf_array']['bmargin']-$GLOBALS['pdf_array']['h_pfoot']-$GLOBALS['pdf_array']['h_foot'];
				$GLOBALS['pdf_array']['max_y']	= $GLOBALS['pdf_array']['min_y']+$GLOBALS['pdf_array']['h_foot'];
				$rh='h_foot';
			}
			if($GLOBALS['pdf_clipping_debug'] || $cdebug)	// Rahmen zu Debugzwecken ausgeben
			{
				fxpdf_linestyle($pdf, 0.0, 0.0, -0.25);
				fxpdf_color($pdf, 'D', 0, '00FF00');
				$pdf->Rect($GLOBALS['pdf_array']['lmargin'], $GLOBALS['pdf_array']['min_y'], $GLOBALS['pdf_array']['w_body'], $GLOBALS['pdf_array'][$rh], 'D');
			}
		break;

		case 2:	// FeRox-Fusszeile
			$GLOBALS['pdf_array']['min_y']	= $GLOBALS['pdf_array']['height']-$GLOBALS['pdf_array']['bmargin']-$GLOBALS['pdf_array']['h_pfoot'];
			$GLOBALS['pdf_array']['max_y']	= $GLOBALS['pdf_array']['min_y']+$GLOBALS['pdf_array']['h_pfoot'];
			$frwidth=$GLOBALS['pdf_array']['w_body'];
			$yp=-5.0;
			$fs=-8.0;
			// Schatten
			fxpdf_linestyle($pdf, 0.0, 0.0, 0.25);
			fxpdf_color($pdf, 'DF', 0, 'eeeeee');
			$pdf->Rect($GLOBALS['pdf_array']['lmargin']+2.25, $GLOBALS['pdf_array']['max_y']+0.25, $frwidth, 2.0, 'DF');
			$pdf->Rect($GLOBALS['pdf_array']['width']-$GLOBALS['pdf_array']['rmargin']+0.25, $GLOBALS['pdf_array']['min_y']+2.25, 2.0, $GLOBALS['pdf_array']['h_pfoot'], 'DF');
			// Rahmen
			fxpdf_color($pdf, 'D', 0, 'bbbbbb');
			fxpdf_color($pdf, 'F', 0, 'f8f8f8');
			$pdf->Rect($GLOBALS['pdf_array']['lmargin'], $GLOBALS['pdf_array']['min_y'], $frwidth, $GLOBALS['pdf_array']['h_pfoot'], 'DF');
			// Text
			// - Programm oder Firmenname
			if(strlen($GLOBALS['pdf_array']['f_text']))
				fxpdf_text($pdf, $GLOBALS['pdf_array']['f_text'], 5.0, $yp, $fs, 'left', 'bold', 0.25, '000000');
			else
			{
				fxpdf_text($pdf, 'fx-project', 5.0, $yp, $fs, 'left', 'bold', 0.25, '000000');
				fxpdf_text($pdf, '(www.fx-project.org)', 50.0, $yp, $fs, 'left', 'normal', 0.25, '000000');
			}
			// - Seitennummer
			if($hf === true)
				fxpdf_text($pdf, $hh, $frwidth/2.0, $yp, $fs, 'center');				// 1A
			else if($hf > 1)
			{
				$seite=meldung(1583, false, $hh, $hf);
				if(strlen($seite))
					fxpdf_text($pdf, $seite, $frwidth/2.0, $yp, $fs, 'center');		// Seite 1 von 2
				else
					fxpdf_text($pdf, "$hh / $hf", $frwidth/2.0, $yp, $fs, 'center');	// 1 / 2
			}
			// - Datum (Mandant+Benutzer)
			if($dmb !== false)
			{
				if(!$dmb)
					$dmb=get_entry(fxNow(), 'datum');
				fxpdf_text($pdf, $dmb, -5.0, $yp, $fs, 'right', 'normal', 0.25, '444444');
			}
		break;

		default:	// Body
			$GLOBALS['pdf_array']['min_y']=$GLOBALS['pdf_array']['tmargin'];
			if($hh && $GLOBALS['pdf_array']['h_head'])	// Kopf
				$GLOBALS['pdf_array']['min_y'] += $GLOBALS['pdf_array']['h_head'];

			$GLOBALS['pdf_array']['max_y']=$GLOBALS['pdf_array']['height']-$GLOBALS['pdf_array']['bmargin']-$GLOBALS['pdf_array']['h_pfoot'];
			if($hf && $GLOBALS['pdf_array']['h_foot'])	// Fuss
				$GLOBALS['pdf_array']['max_y'] -= $GLOBALS['pdf_array']['h_foot'];

			$GLOBALS['pdf_array']['h_body']	= $GLOBALS['pdf_array']['max_y']-$GLOBALS['pdf_array']['min_y'];
			$GLOBALS['pdf_array']['w_body']	= $GLOBALS['pdf_array']['max_x']-$GLOBALS['pdf_array']['min_x'];
			if($GLOBALS['pdf_clipping_debug'] || $cdebug)	// Rahmen zu Debugzwecken ausgeben
			{
				fxpdf_linestyle($pdf, 0.0, 0.0, 0.25);
				fxpdf_color($pdf, 'D', 0, '008080');
				$pdf->Rect($GLOBALS['pdf_array']['min_x'], $GLOBALS['pdf_array']['min_y'], $GLOBALS['pdf_array']['w_body'], $GLOBALS['pdf_array']['h_body'], 'D');
			}
		break;
	}
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * + FXPDF_USER																	+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf      - Mandatory parameter: ???PARAMETER???
 * @param various $orderline - Mandatory parameter: ???PARAMETER???
 */
function fxpdf_user(&$pdf, $orderline)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$kommandos=explode('^', $orderline);
	if(fxIsArray($kommandos))
	{
		foreach($kommandos as $k)
		{
			$type=strtoupper(substr($k, 0,1));
			$params=explode('|', substr($k, 1));
			if(is_array($params))
			{
				$sp=sizeof($params);
				switch($type)
				{
					case 'T':	// Text: fxpdf_text($pdf, $text, $xc, $yc, $sf=8.0, $align='left', $mode='normal', $lw=0.25, $rgb='000000', $deg=0, $space=0.0)
						if($sp >= 3)
						{
							$text=fxpdf_convert($params[0]);

							if(!$params[3])			// $sf:		Fontgrösse
								$params[3]=8.0;
							if(!$params[4])			// $align:	Ausrichtung
								$params[4]='left';
							if(!$params[5])			// $mode:	Zeichenmodus
								$params[5]='normal';
							if(!$params[6])			// $lw:		Strichstärke
								$params[6]=0.25;
							if(!$params[7])			// $rgb:	Farbe im Format RRGGBB
								$params[7]='000000';
							if(!$params[8])			// $deg:	Rotationswinkel in Grad
								$params[8]=0;
							if(!$params[9])			// $space:	Zusatzabstand bei Zeilenumbrüchen
								$params[9]=0.0;
							fxpdf_text($pdf, $text, $params[1], $params[2], $params[3], $params[4], $params[5], $params[6], $params[7], $params[8], $params[9]);
						}
					break;

					case 'P':	// Bild: fxpdf_picture($pdf, $pic, $xc, $yc, $sf=1.0, $align='left', $pw='', $ph='', $frgb='')
						if($sp >= 3)
						{
							if(!$params[3])			// $sf:		Skalierungsfaktor
								$params[3]=1.0;
							if(!$params[4])			// $align:	Ausrichtung
								$params[4]='left';
							if(!$params[5])			// $pw:		Bildbreite
								$params[5]='';
							if(!$params[6])			// $ph:		Bildhöhe
								$params[6]='';
							if(!$params[7])			// $frgb:	Rahmen
								$params[7]='';
							fxpdf_picture($pdf, $params[0], $params[1], $params[2], $params[3], $params[4], $params[5], $params[6], $params[7]);
						}
					break;

					case 'R':	// Rechteck: fxpdf_rec($pdf, $x1, $y1, $x2, $y2, $wd=0.0, $bd=0.0, $lw=0.35, $frgb='000000')
						if($sp >= 4)
						{
							if(!$params[4])			// $wd:		Weisse Punkte
								$params[4]=0.0;
							if(!$params[5])			// $bd:		Schwarze Punkte
								$params[5]=0.0;
							if(!$params[6])			// $lw:		Strichstärke
								$params[6]=0.35;
							if(!$params[7])			// $frgb:	Farbe im Format RRGGBB
								$params[7]='000000';
							fxpdf_rec($pdf, $params[0], $params[1], $params[2], $params[3], $params[4], $params[5], $params[6], $params[7]);
						}
					break;

					case 'B':	// Box: fxpdf_box($pdf, $x1, $y1, $x2, $y2, $rgb='ffffff', $fb='1111', $wd=0.0, $bd=0.0, $lw=0.35, $frgb='000000')
						if($sp >= 4)
						{
							if(!$params[4])			// $rgb:	Füllfarbe im Format RRGGBB
								$params[4]='ffffff';
							if(!$params[5])			// $fb:		Rahmen
								$params[5]='1111';
							if(!$params[6])			// $wd:		Weisse Punkte
								$params[6]=0.0;
							if(!$params[7])			// $bd:		Schwarze Punkte
								$params[7]=0.0;
							if(!$params[8])			// $sf:		Strichstärke
								$params[8]=0.35;
							if(!$params[9])			// $frgb:	Rahmenfarbe im Format RRGGBB
								$params[9]='000000';
							fxpdf_box($pdf, $params[0], $params[1], $params[2], $params[3], $params[4], $params[5], $params[6], $params[7], $params[8], $params[9]);
						}
					break;

					case 'B':	// Gradient Box: fxpdf_gbox($pdf, $x1, $y1, $x2, $y2, $rgb1='ffffff', $rgb2='ffffff', $bbox=false)
						if($sp >= 4)
						{
							if(!$params[4])			// $rgb1:	Füllfarbe 1 im Format RRGGBB
								$params[4]='ffffff';
							if(!$params[5])			// $rgb2:	Füllfarbe 2 im Format RRGGBB
								$params[5]='ffffff';
							if(!$params[6])			// $bbox:	Box
								$params[6]=false;
							fxpdf_gbox($pdf, $params[0], $params[1], $params[2], $params[3], $params[4], $params[5], $params[6]);
						}
					break;

					case 'L':	// Linie: fxpdf_line($pdf, $x1, $y1, $x2, $y2, $wd=0.0, $bd=0.0, $lw=0.35, $rgb='000000')
						if($sp >= 4)
						{
							if(!$params[4])			// $wd:		Weisse Punkte
								$params[4]=0.0;
							if(!$params[5])			// $bd:		Schwarze Punkte
								$params[5]=0.0;
							if(!$params[6])			// $lw:		Strichstärke
								$params[6]=0.35;
							if(!$params[7])			// $rgb:	Farbe im Format RRGGBB
								$params[7]='000000';
							fxpdf_line($pdf, $params[0], $params[1], $params[2], $params[3], $params[4], $params[5], $params[6], $params[7]);
						}
					break;
				}
			}
		}
	}
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * + FXPDF_WRAPTEXT																+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf      - Mandatory parameter:                ???PARAMETER???
 * @param various $text      - Mandatory parameter:                ???PARAMETER???
 * @param various $fnts      - Mandatory parameter:                ???PARAMETER???
 * @param various $style     - Mandatory parameter:                ???PARAMETER???
 * @param various $mwidth    - Mandatory parameter:                ???PARAMETER???
 * @param boolean $newmethod - Optional parameter (default=true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fxpdf_wraptext(&$pdf, $text, $fnts, $style, $mwidth, $newmethod=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($text))
		return '';

	$text=fxpdf_convert($text);
	$wrap_debug=false;
	if($wrap_debug)
		echo('<div style="margin-right:16px;background:#eee;color:#006b9f;padding:8px;border:1px solid black;border-radius:4px;"><b>Originaltext:</b><pre style="white-space:pre-wrap;">'.fxHtmlEncode($text).'</pre></div>');

	$nltr=array("\r\n"=>'<:nl:>', "\r"=>'<:nl:>', "\n"=>'<:nl:>');
	$rtxt_array=explode('<:nl:>', strtr($text, $nltr));
	if(!is_array($rtxt_array) || !sizeof($rtxt_array))
		return $text;

	$text_array=array();
	$text_cnt=0;

	if(strlen($style))
	{
		$style=strtoupper(substr($style,0,1));
		if($style == 'N')
			$style='';
	}

	fxpdf_font($pdf, $fnts, $style);

	foreach($rtxt_array as $rtxt)
	{
		$lpt=$pdf->GetStringWidth($rtxt);
		if($lpt <= $mwidth)
			$text_array[$text_cnt++]=$rtxt;
		else
		{
			$rol=0;
			$rsl=strlen($rtxt);
			while($rsl && ($rsl != $rol))
			{
				$rol=$rsl;
				if($lpt <= $mwidth)
				{
					$text_array[$text_cnt++]=$rtxt;
					break;
				}

				if($newmethod)
				{
					$ospp=0;
					$ctxt='';
					while(true)
					{
						$spp1=strpos($rtxt, ' ',$ospp);
						$spp2=strpos($rtxt, '-',$ospp);
						$spp3=strpos($rtxt, '. ',$ospp);
						if(($spp1 !== false) || ($spp2 !== false) || ($spp3 !== false))
						{
							$spp=strlen($rtxt);
							if($spp1 !== false)
								$spp=$spp1;
							if(($spp2 !== false) && ($spp2 < $spp))
								$spp=$spp2;
							if(($spp3 !== false) && ($spp3 < $spp))
								$spp=$spp3;

							$ntxt=mb_substr($rtxt, 0,$spp);
							$lpt=$pdf->GetStringWidth($ntxt);
							if($lpt > $mwidth)
							{
								if($ospp && strlen($ctxt))
								{
									$rtxt=mb_substr($rtxt, $ospp);
									break;
								}
								else
								{
									while($lpt > $mwidth)
									{
										--$spp;
										$ntxt=mb_substr($rtxt, 0,$spp);
										$lpt=$pdf->GetStringWidth($ntxt);
									}
									$rtxt=mb_substr($rtxt, $spp);
									$ctxt=$ntxt;
									break;
								}
							}
							else
							{
								$ctxt=$ntxt;
								$ospp=$spp+1;
							}
						}
						else if($ospp && strlen($ctxt))
						{
							$rtxt=mb_substr($rtxt, $ospp);
							break;
						}
						else if(!$ospp)
						{
							$spp=strlen($rtxt);
							if($spp == 1)
							{
								$ctxt=$rtxt;
								$rtxt='';
							}
							else
							{
								while(($spp > 1) && ($lpt > $mwidth))
								{
									--$spp;
									$ntxt=mb_substr($rtxt, 0,$spp);
									$lpt=$pdf->GetStringWidth($ntxt);
								}
								$rtxt=mb_substr($rtxt, $spp);
								$ctxt=$ntxt;
							}
							break;
						}
						else
						{
							$ctxt=$rtxt;
							$rtxt='';
							break;
						}
					}
				}
				else
				{
					$ctxt=$rtxt;
					$lpt=$pdf->GetStringWidth($ctxt);
					$sl=strlen($ctxt)-1;
					$rtxt='';

					while($sl && ($lpt > $mwidth))
					{
						$sl--;
						// Letztes Leerzeichen suchen
						while($sl && (mb_substr($ctxt, $sl,1) != ' '))
							$sl--;
						if(mb_substr($ctxt, $sl,1) == ' ')
						{
							if($rtxt)
								$rtxt=' '.$rtxt;
							$rtxt=mb_substr($ctxt, $sl+1).$rtxt;
							$ctxt=mb_substr($ctxt, 0,$sl);
						}
						else
						{
							$rtxt=mb_substr($ctxt, -1).$rtxt;
							$ctxt=mb_substr($ctxt, 0,-1);
						}
						$lpt=$pdf->GetStringWidth($ctxt);
					}
				}

				$text_array[$text_cnt++]=$ctxt;
				$lpt=$pdf->GetStringWidth($rtxt);
				$rsl=strlen($rtxt);
			}
		}
	}

	if($wrap_debug)
		fxDebug($text_array, 'Rückgabe-Array $text_array');

	return $text_array;
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * + FXPDF_STRLEN																+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf   - Mandatory parameter:                ???PARAMETER???
 * @param various $str    - Mandatory parameter:                ???PARAMETER???
 * @param various $fnts   - Mandatory parameter:                ???PARAMETER???
 * @param string  $style  - Optional parameter (default=''):  ???PARAMETER???
 * @param string  $mwidth - Optional parameter (default=''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fxpdf_strlen(&$pdf, $str, $fnts, $style='', $mwidth='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($str) || (strlen($mwidth) && ((float)$mwidth<=0.0)))
	{
		if(strlen($mwidth))
			return '';
		else
			return 0;
	}

	if(strlen($style))
	{
		$style=strtoupper(substr($style,0,1));
		if($style == 'N')
			$style='';
	}

	fxpdf_font($pdf, $fnts, $style);

	$lpt=$pdf->GetStringWidth($str);
	if($lpt < 2.0)
		$lpt=	strlen($str)*($fnts/1.75);
	if($mwidth)
	{
		while($lpt > $mwidth)
		{
			$str=mb_substr($str, 0,-1);
			$lpt=$pdf->GetStringWidth($str);
			if($lpt < 2.0)
				$lpt=strlen($str)*($fnts/1.75);
		}
		return $str;
	}
	else
		return $lpt;
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * + FXPDF_FONT																	+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf  - Mandatory parameter:                 ???PARAMETER???
 * @param double  $sf    - Optional parameter (default=8.0):  ???PARAMETER???
 * @param string  $style - Optional parameter (default=''):   ???PARAMETER???
 */
function fxpdf_font(&$pdf, $sf=8.0, $style='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sf=get_double($sf);
	if($sf < 0.0)
		$sf=-$sf;
	else if($GLOBALS['pdf_array']['ffac'] > 0.0)
		$sf *= $GLOBALS['pdf_array']['ffac'];

	$fonttype='Helvetica';
	if(isset($GLOBALS['pdf_array']['font']))
		$fonttype=$GLOBALS['pdf_array']['font'];

	$pdf->SetFont($fonttype, $style, $sf);
if(isset($GLOBALS['pdf_debug_methodcalls']) && $GLOBALS['pdf_debug_methodcalls']) echo('<div style="background:#fff;padding:3px;">$rv = $pdf-><b>SetFont</b>(\''.$fonttype.'\', \''.$style.'\', '.$sf.');</div>'.$GLOBALS['nl']);
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * + FXPDF_DIAGONAL																+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf - Mandatory parameter:                Direct reference to PDF class
 * @param various $text - Optional parameter (default=''):  Text to be written
 */
function fxpdf_diagonal(&$pdf, $text='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(strlen($text))
	{
		$text=fxpdf_convert($text);

		$tv_wk=rad2deg(atan($GLOBALS['pdf_array']['height'] / $GLOBALS['pdf_array']['width']));
		$tv_ml=sqrt(($GLOBALS['pdf_array']['height']*$GLOBALS['pdf_array']['height'])+($GLOBALS['pdf_array']['width']*$GLOBALS['pdf_array']['width']))-300.0;
		$tv_fs=strlen($text)*18.0;

		$pdf->SetFontSize($tv_fs);
		$tv_sl=$pdf->GetStringWidth($text);
		if($tv_sl < 2.0)
			$tv_sl=strlen($text)*($tv_fs/1.75);
		while($tv_sl > $tv_ml)
		{
			$tv_fs -= 1.0;
	    	$pdf->SetFontSize($tv_fs);
			$tv_sl=$pdf->GetStringWidth($text);
			if($tv_sl < 2.0)
				$tv_sl=strlen($text)*($tv_fs/1.75);
		}

		$xc=($GLOBALS['pdf_array']['width']-$tv_sl)/2;
		$yc=($GLOBALS['pdf_array']['height']+$tv_fs)/2;
		if(fxpdf_mode() == FXPDF_TCPDF)
			$pdf->StartTransform();
		$pdf->Rotate($tv_wk, $GLOBALS['pdf_array']['width']/2, $GLOBALS['pdf_array']['height']/2);
		fxpdf_linestyle($pdf, 0.0, 0.0, 0.25);
		fxpdf_color($pdf, 'T', 0, 'ffffff');
		fxpdf_color($pdf, 'S', 0, 'ff0000');
		if(fxpdf_mode() == FXPDF_TCPDF)
		{
			$pdf->Text($xc, $yc, $text, 0.25, false, false, 0, 0, 'L', 0, '', 0, false, 'L', 'M');
			$pdf->StopTransform();
		}
		else
		{
			$pdf->Text($xc, $yc, $text, 1);
			$pdf->Rotate(0);
		}
	}
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * + FXPDF_APPEND																+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf   - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $append - Optional parameter (default=false):  ???PARAMETER???
 */
function fxpdf_append(&$pdf, $append=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$text_append=meldung(1588, false);	// Anhang

	if(fxIsArray($GLOBALS['fxptdata']['_pdf_append']) && ($GLOBALS['pdf_array']['att'] || $append))
	{
		$pdf_array_backup=$GLOBALS['pdf_array'];

		$GLOBALS['pdf_array']['width']		=	595.353535352;
		$GLOBALS['pdf_array']['height']		=	841.999999997;
		$GLOBALS['pdf_array']['lmargin']	=	56.7003367002;
		$GLOBALS['pdf_array']['rmargin']	=	28.3501683501;
		$GLOBALS['pdf_array']['tmargin']	=	28.3501683501;
		$GLOBALS['pdf_array']['bmargin']	=	28.3501683501;
		$GLOBALS['pdf_array']['fac']		=	1.00;
		$GLOBALS['pdf_array']['lsfmt']		=	0;
		$GLOBALS['pdf_array']['fheight']	=	8.00;
		$GLOBALS['pdf_array']['ffheight']	=	$GLOBALS['pdf_array']['fheight']*$GLOBALS['pdf_array']['fac'];

		// Neue Seite im DIN-A4 Hochformat erstellen.
		fxpdf_page($pdf, 'begin', array($GLOBALS['pdf_array']['width'], $GLOBALS['pdf_array']['height']));
		$pdf->SetMargins($GLOBALS['pdf_array']['lmargin'], $GLOBALS['pdf_array']['tmargin'], $GLOBALS['pdf_array']['rmargin']);

		// Body
		fxpdf_clipping($pdf, 0, false, true);
//fxDebug($GLOBALS['pdf_array'], 'pdf_array (NEW)');

		// Überschrifts-Box
		fxpdf_box($pdf, 4.00, 4.00, -1.00, 33.00, 'bbbbbb', '0000');
		fxpdf_gbox($pdf, 1.00, 1.00, -4.00, 30.00, 'ffffff', 'dddddd', array(0,1,0,0));
		fxpdf_rec($pdf, 1.00, 1.00, -4.00, 30.00, 0.00, 0.00, 0.25, '000000');
		// Text: Anhang
		fxpdf_text($pdf, $text_append, 8.00, 20.00, -14.00, 'left', 'shadow', -0.25, '000000');

		// Alle Tooltips durchgehen
		$ttda	= array();
		$apage	= 0;
		$yt		= 60.00;
		$cwidth	= 426.00;
		$xl		= 1.00;
		$x		= $xl;
		$title	= '';
		$trans_html=array_flip(get_html_translation_table(HTML_ENTITIES));
		$trans_html['&nbsp;']=chr(32);
		while(true)
		{
			$y=$yt;
			$finished=true;
			foreach($GLOBALS['fxptdata']['_pdf_append'] as $an => $data)
			{
				if($finished && !isset($ttda[$an]))
				{
					$type=strtolower(trim(key($data)));
					$value=$data[$type];
					$set=true;
//echo('<b>'.$an.':</b> $type='.$type.', $value='.$value.'<br>');

					switch($type)
					{
						case 'title':
							$title=$value;
						break;

						case 'head':
							if(($apage > 0) && ($y > $yt))
							{
								$set=false;
								$y=$GLOBALS['pdf_array']['h_body'];
							}
							else
							{
								if(strlen($title))
								{
									fxpdf_text($pdf, $title, $x, $y, -16.00, 'left', 'bold', -0.25, '000000');
									$y += 4.00*$GLOBALS['pdf_array']['ffheight'];
								}
								fxpdf_rec($pdf, $x, $y+1.00, -1.00, $y+1.00, 0.00, 0.00, 0.25, '000000');
								fxpdf_text($pdf, $value, $x, $y, -10.00, 'left', 'bold', -0.25, '000000');
								$y += 2.00*$GLOBALS['pdf_array']['ffheight'];
							}
						break;

						case 'hspace':
							$y += get_double($value)*$GLOBALS['pdf_array']['ffheight'];
						break;

						case 'vspace':
							if($value)
								$x += get_double($value);
							else
								$x=$xl;
						break;

						default:
							$xa=0.00;
							$xap=strpos($type,'_');
							if($xap !== false)
								$xa=get_double(substr($type,$xap+1));
							$lp=strpos($value,'^');
							if($lp !== false)
							{
								$lit=mb_substr($value,0,$lp);
								fxpdf_text($pdf, $lit.':', $x+2.00, $y, $GLOBALS['pdf_array']['fheight'], 'left', 'bold', 0.20, '000000');
								$value=mb_substr($value,$lp+1);
							}
							fxpdf_text($pdf, $value, $x+2.00+$xa, $y, $GLOBALS['pdf_array']['fheight'], 'left', 'normal', 0.001, '000000');
							$y += 1.50*$GLOBALS['pdf_array']['ffheight'];
						break;
					}

					if($set)
						$ttda[$an]=true;

					if($y > ($GLOBALS['pdf_array']['h_body']-(2.0*$GLOBALS['pdf_array']['ffheight'])))
					{
						$finished=false;
						break;
					}
				}
			}

			// FeRox-Fusszeile setzen, falls nicht abgeschaltet
			if(!isset($GLOBALS['pdf_array']['f_nofoot']))
			{
				$fptext=$text_append.' '.fxpdf_char($apage);
				fxpdf_clipping($pdf, 2, $fptext, true, $dmb);
			}

			if($finished)
				break;

			fxpdf_page($pdf, 'begin', array($GLOBALS['pdf_array']['width'], $GLOBALS['pdf_array']['height']));
			fxpdf_clipping($pdf, 0, false, true);
			$apage++;
		}

		$GLOBALS['pdf_array']=$pdf_array_backup;
	}
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * + FXPDF_ANHANG																+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf - Mandatory parameter: ???PARAMETER???
 * @param various $ntt  - Mandatory parameter: ???PARAMETER???
 */
function fxpdf_anhang(&$pdf, $ntt)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// PDF-Anhang
	if(fxIsArray($ntt))
	{
//fxDebug($ntt, '$ntt');
		$seite=1;
		$anhang=meldung(1588).':';	// 1588: Anhang

		$oa=$GLOBALS['pdf_array'];

		$pw=min($GLOBALS['pdf_array']['width'], $GLOBALS['pdf_array']['height']);
		$ph=max($GLOBALS['pdf_array']['width'], $GLOBALS['pdf_array']['height']);

		$GLOBALS['pdf_array']['width']	= $pw;
		$GLOBALS['pdf_array']['height']	= $ph;
		$GLOBALS['pdf_array']['fac']	= 1.0;
		$GLOBALS['pdf_array']['lsfmt']	= 0;

		$fontsize=8.0;
		$xv=0;

		$lc=0;
		$lcm=sizeof($ntt);
		while($lc < $lcm)	// Alle Tooltipzeilen durchgehen
		{
			// Seite beginnen
			fxpdf_page($pdf, 'begin', array($pw, $ph));
			fxpdf_clipping($pdf, 0, true, false);

			// Allgemeine Variablen
			if(!$xv)
			{
				$len_nummer		= 0.0;
				$len_literal	= 0.0;
				$len_wert		= 0.0;
				// Maximallängen ermitteln
				foreach($ntt as $nttdat)
				{
					// Nummer
					$text=$nttdat['head'];
					if(mb_substr($text, 0,1) == '^')
						$text=(int)mb_substr($text, 1,4).'. '.(int)mb_substr($text, 5,4).'.';
					$len_nummer=max($len_nummer, fxpdf_strlen($pdf, $text, $fontsize, 'B'));

					if(is_array($nttdat['tip']))
					{
						foreach($nttdat['tip'] as $tipdat)
						{
							// Literal
							if(isset($tipdat['lit']))
								$len_literal=max($len_literal, fxpdf_strlen($pdf, $tipdat['lit'], $fontsize));
							// Wert
							if(isset($tipdat['val']))
								$len_wert=max($len_wert, fxpdf_strlen($pdf, $tipdat['val'], $fontsize));
						}
					}
				}
//echo('<B>$len_nummer:</B> '.$len_nummer.' -- <B>$len_literal:</B> '.$len_literal.' -- <B>$len_wert:</B> '.$len_wert.'<BR>');

				// Abstandsvariablen definieren
				$xl=25.0+$len_nummer;
				if($len_literal)
					$xv=$xl+$len_literal+10.0;
				else
					$xv=$xl;
				$xe=$xv+$len_wert;
//echo('<B>$xl:</B> '.$xl.' -- <B>$xv:</B> '.$xv.' -- <B>$xe:</B> '.$xe.' -- <B>w_body:</B> '.$GLOBALS['pdf_array']['w_body'].'<BR>');
				if($xe > $GLOBALS['pdf_array']['w_body'])
				{
					$xnw=$len_nummer+$len_wert;
					$xdiff=$xe-$GLOBALS['pdf_array']['w_body'];
					$len_nummer=$len_nummer - ($xdiff * ($len_nummer / $xnw));
					$xl=25.0+$len_nummer;
					if($len_literal)
						$xv=$xl+$len_literal+10.0;
					else
						$xv=$xl;
					$len_wert=max(0.0, $GLOBALS['pdf_array']['w_body']-$xv);
				}
//echo('<B>$len_nummer:</B> '.$len_nummer.' -- <B>$len_literal:</B> '.$len_literal.' -- <B>$len_wert:</B> '.$len_wert.'<BR>');
			}

			// Titel
			if($seite == 1)
			{
				fxpdf_text($pdf, $anhang, 1.0, 14.0, 14.0, 'left', 'bold');
				fxpdf_line($pdf, 1.0, 17.0, -1.0, 17.0);
				$yc=30.0;
			}
			else
				$yc=10.0;
			$ycs=$yc;
			$xco=1.0;

			$anh_again=false;
			while(($lc < $lcm) && ($yc < $GLOBALS['pdf_array']['h_body']))
			{
				$text=$ntt[$lc]['head'];
				if(mb_substr($text, 0,1) == '^')
					$text=(int)mb_substr($text, 1,4).'. '.(int)mb_substr($text, 5,4).'.';
				$mode=$ntt[$lc]['mode'];
				if(!$mode)
					$mode='bold';
				$text=fxpdf_strlen($pdf, $text, $fontsize, $mode, $len_nummer);
				fxpdf_text($pdf, $text, $xco, $yc, $fontsize, 'left', $mode);
				$yce=$yc;
				$ly=$yc-$fontsize;
//echo('<B>$ly:</B> '.$ly.' -- <B>h_body:</B> '.$GLOBALS['pdf_array']['h_body'].'<BR>');
				if($ly > $GLOBALS['pdf_array']['h_body'])
					$anh_again=true;
				else
				{
					// Informationen
					if(fxIsArray($ntt[$lc]['tip']))
					{
						foreach($ntt[$lc]['tip'] as $ttl => $ttldat)
						{
							if($yc > $GLOBALS['pdf_array']['h_body'])
							{
								$anh_again=true;
								break;
							}
							// Literal
							if(isset($ttldat['lit']))
							{
								$text=fxpdf_strlen($pdf, $ttldat['lit'], $fontsize, '', $len_literal);
								fxpdf_text($pdf, $text, $xco+$xl, $yc, $fontsize);
							}
							// Projektkategorie
							if(isset($ttldat['cat']))
								fxpdf_box($pdf, $xco+$xv, $yc-$fontsize*0.75, $xco+$xv+3.0*$GLOBALS['pdf_array']['fac'], $yc, $ttldat['cat'], '0000');
							$txa=explode('*br*', $ttldat['val']);
							if(fxIsArray($txa))
							{
								foreach($txa as $txn => $txl)
								{
									$txl=trim($txl);
									if(strlen($txl))
									{
										$text=fxpdf_strlen($pdf, $txl, $fontsize, '', $len_wert);
										fxpdf_text($pdf, $text, $xco+$xv, $yc, $fontsize, 'left', 'normal', 0.25, '888888');
										if(isset($txa[$txn+1]) && strlen(trim($txa[$txn+1])))
											$yc += ($fontsize+1.0);
									}
								}
							}
							$yce=$yc;
							$yc += ($fontsize+1.0);
						}
					}
					$yc += ($fontsize*2.0);
				}
				if(($yce > $GLOBALS['pdf_array']['h_body']) || $anh_again)
				{
					fxpdf_box($pdf, $xco, $ly, $xe, $yc, 'ffffff', '0000');
					// Prüfen, ob neue Spalte möglich
					$nxo=($GLOBALS['pdf_array']['w_body']/2.0)+1.0;
					$nxe=$nxo+$xe;
					//echo('<B>$nxo:</B> '.$nxo.' -- <B>$nxe:</B> '.$nxe.' -- <B>w_body:</B> '.$GLOBALS['pdf_array']['w_body'].'<BR>');
					if(($xco < $nxo) && ($nxe <= $GLOBALS['pdf_array']['w_body']))
					{
						$xco=$nxo;
						$yc=$ycs;
						$anh_again=false;
					}
				}
				else
					$lc++;
			}

			// Vorher noch die FeRox-Fusszeile setzen
			fxpdf_clipping($pdf, 2, strtoupper(mb_substr($anhang, 0,1)).$seite, true, $GLOBALS['dmb']);
			fxpdf_page($pdf, 'end');

			$seite++;
		}

		$GLOBALS['pdf_array']=$oa;
	}
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  FXPDF_COLOR																+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various &$pdf    - Mandatory parameter: ???PARAMETER???
 * @param various $mode    - Mandatory parameter: ???PARAMETER???
 * @param various $lighten - Mandatory parameter: ???PARAMETER???
 * @param various $rgb     - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fxpdf_color(&$pdf, $mode, $lighten, $rgb)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$lrgb=fxpdf_lighten($lighten, $rgb);
	$drgb=hexdec($lrgb);
	$r=($drgb & 0xff0000) >> 16;
	$g=($drgb & 0x00ff00) >> 8;
	$b=($drgb & 0x0000ff);
//echo('<b>fxpdf_color:</b> '.$rgb.' ('.$lighten.') -> '.$lrgb.' ('.$drgb.') --&gt; ['.$r.', '.$g.', '.$b.']<br>');

	// In Grauwert umwandeln (in PDF-Konfigurationsfile einstellbar)
	if($GLOBALS['pdf_array']['grey'])
	{
		$grey_rgb=($r*0.30 + $g*0.59 + $b*0.11) & 0xFF;
		$r=$g=$b=$grey_rgb;
//echo('&rarr; <b>grey:</b> ['.$r.', '.$g.', '.$b.']<br>');
	}

	if(!strlen($mode))
		$mode='DFT';

	$fm=strtoupper($mode);
	if($fm == 'A')							// Array
		return array($r, $g, $b);

	$fl=strlen($fm);
	for($f=0; $f<$fl; $f++)
	{
		$fc=substr($fm, $f, 1);
		if(($fc == 'D') || ($fc == 'S'))		// Draw/Stroke
			$pdf->SetDrawColor($r, $g, $b);
		else if($fc == 'F')						// Fill
			$pdf->SetFillColor($r, $g, $b);
		else if($fc == 'T')						// Text
			$pdf->SetTextColor($r, $g, $b);
	}
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  FXPDF_LIGHTEN																+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various $lighten - Mandatory parameter: ???PARAMETER???
 * @param various $rgb     - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fxpdf_lighten($lighten, $rgb)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$rgb=hexdec($rgb);
	$c[0]=($rgb & 0xFF0000) >> 16;
	$c[1]=($rgb & 0x00FF00) >> 8;
	$c[2]=($rgb & 0x0000FF);

	for($i=0; $i<3; $i++)
	{
		if($lighten > 0)
			$c[$i]=min(255, $c[$i]+$lighten);
		else if($lighten < 0)
			$c[$i]=max(0, $c[$i]+$lighten);
	}
	$ncol=substr('00'.dechex($c[0]), -2).substr('00'.dechex($c[1]), -2).substr('00'.dechex($c[2]), -2);

	return $ncol;
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  FXPDF_GETX																+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various $xc - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fxpdf_getx($xc)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// X-Koordinate
	$xc1=mb_substr($xc, 0,1);
	if($xc1 == '&')	// Absolut-Koordinaten
	{
		$vz=mb_substr($xc, 1,1);
		if($vz != '-')
			$vz='';
		$xc=(float)mb_substr($xc, 1);
		if($vz || ($xc < 0))	// vom rechten Seitenrand aus
			$xc=$GLOBALS['pdf_array']['width']+$xc;
	}
	else if($xc1 == '!')	// Relative-Koordinaten-1 (inkl. Berücksichtigung des linken Seitenrahmens)
		$xc=$GLOBALS['pdf_array']['lmargin']+(float)mb_substr($xc, 1);
	else if($xc1 == '§')	// Relative-Koordinaten-2 (ohne Berücksichtigung des linken Seitenrahmens)
		$xc=(float)mb_substr($xc, 1);
	else
	{
		$vz=mb_substr($xc, 0,1);
		if($vz != '-')
			$vz='';
		if($vz || ($xc < 0))	// vom rechten Rahmen aus
			$xc=$GLOBALS['pdf_array']['width']-$GLOBALS['pdf_array']['rmargin']+$xc;
		else
			$xc=$GLOBALS['pdf_array']['lmargin']+$xc;
	}

	return $xc;
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * +  FXPDF_GETY																+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various $yc - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fxpdf_gety($yc)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Y-Koordinate
	$yc1=mb_substr($yc, 0,1);
	if($yc1 == '&')	// Absolut-Koordinaten
	{
		$vz=mb_substr($yc, 1,1);
		if($vz != '-')
			$vz='';
		$yc=(float)mb_substr($yc, 1);
		if($vz || ($yc < 0))	// vom unteren Rand aus
			$yc=$GLOBALS['pdf_array']['height']+$yc;
	}
	else if($yc1 == '!')	// Relative-Koordinaten-1 (inkl. Berücksichtigung des oberen Seitenrahmens)
		$yc=$GLOBALS['pdf_array']['min_y']+(float)mb_substr($yc, 1);
	else if($yc1 == '§')	// Relative-Koordinaten-2 (ohne Berücksichtigung des oberen Seitenrahmens)
		$yc=(float)mb_substr($yc, 1);
	else
	{
		$vz=mb_substr($yc, 0,1);
		if($vz != '-')
			$vz='';
		if($vz || ($yc < 0))	// vom unteren Rand aus
			$yc=$GLOBALS['pdf_array']['max_y']+$yc;
		else
			$yc=$GLOBALS['pdf_array']['min_y']+$yc;
	}

	return $yc;
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * + FXPDF_FOOT_INFO															+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @return ???RETURN???
 */
function fxpdf_foot_info()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(isset($GLOBALS['pdf_array']['f_nodate']) && isset($GLOBALS['pdf_array']['f_nocreator']))
		$dmb=false;
	else
	{
		$dmb='';
		if(!isset($GLOBALS['pdf_array']['f_nodate']))
			$dmb=get_entry(fxNow(), 'datum');
		if(!isset($GLOBALS['pdf_array']['f_nocreator']))
		{
			if(!isset($GLOBALS['pdf_array']['f_nodate']))
				$dmb .= ' [';
			$mid=$GLOBALS['fxpglobals']['client'];
			$pid=$GLOBALS['fxpglobals']['person'];
			if($mid || $pid)
			{
				$dmb .= $mid;
				if($pid)
				{
					$bname=db_values("SELECT pname, vorname FROM personen WHERE personen_id=".$pid." AND mandanten_id=".$mid);
					$nm=$bname['pname'];
					if($bname['vorname'])
						$nm=$bname['vorname'].' '.$nm;
					$dmb .= ': '.$nm;
				}
			}
			if(!isset($GLOBALS['pdf_array']['f_nodate']))
				$dmb .= ']';
		}
	}

	return $dmb;
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * + FXPDF_CHAR																	+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various $i - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fxpdf_char($i)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$i=(int)$i;
	if($i > 25)
		return chr(64+(int)($i/26)).chr(65+($i%26));
	return chr(65+$i);
}

/**
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * + FXPDF_CONVERT																+
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * @param various $text - Mandatory parameter:  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fxpdf_convert($text)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

if(isset($GLOBALS['pdf_debug_converter']) && $GLOBALS['pdf_debug_converter']) echo('<div style="background:#fff;border:1px dotted black;padding:3px;"><span class="grey">'.fxHtmlEncode($text).'</span><br /><span class="lightergrey">&rarr;</span>&nbsp;');
	if(function_exists('mb_detect_encoding'))
	{
		$oe=mb_detect_encoding($text, FXP_CHARSET.', CP1252', true);
		if(strlen($oe) && (strtoupper($oe) != 'CP1252'))
			$text=mb_convert_encoding($text, 'CP1252', $oe);
	}
if(isset($GLOBALS['pdf_debug_converter']) && $GLOBALS['pdf_debug_converter']) echo('<b class="blue">'.fxHtmlEncode($text).'</b></div>'.$GLOBALS['nl']);

	return $text;
}

/**
 * ???FUNCTION???
 *
 * @param various $x1 - Mandatory parameter: ???PARAMETER???
 * @param various $y1 - Mandatory parameter: ???PARAMETER???
 * @param various $x2 - Mandatory parameter: ???PARAMETER???
 * @param various $y2 - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fxpdf_is_visible($x1, $y1, $x2, $y2)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($x1 > $x2)
	{
		$xr=$x1;
		$x1=$x2;
		$x2=$xr;
	}
	if($y1 > $y2)
	{
		$yr=$y1;
		$y1=$y2;
		$y2=$yr;
	}
	if(($y1 <= $GLOBALS['pdf_array']['h_body']) && ($y2 >= 0.00) && ($x1 <= $GLOBALS['pdf_array']['w_body']) && ($x2 >= 0.00))
		return true;
	return false;
}
?>