<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : pdf.inc                                                      //
// Version     : 21.2                                                         //
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
 * Check if PDF is available
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;

// FPDI namespaces
use setasign\Fpdi\Fpdi;
use setasign\Fpdi\PdfReader;

// FPDF font subdirectory
$_ext_fpdf_fontpath='';
if(isset($GLOBALS['_ext_array']['FPDF']))
{
	$_fpdf_path=$GLOBALS['_ext_array']['FPDF'];
	while(strlen($_fpdf_path))
	{
		$_fpdf_lsp=strrpos($_fpdf_path,'/');
		if($_fpdf_lsp === false)
			break;
		$_fpdf_path=substr($_fpdf_path,0,$_fpdf_lsp);
		if(is_dir($_fpdf_path.'/font'))
		{
			$_ext_fpdf_fontpath=$_fpdf_path.'/font/';
			break;
		}
	}
}
define('FPDF_FONTPATH', $_ext_fpdf_fontpath);
//echo('FPDF_FONTPATH: '.FPDF_FONTPATH.'<br />');

// FPDF or TCPDF available?
$GLOBALS['_ext_pdf']='';
// ...FPDF?
if(!isset($GLOBALS['_ext_pdf_tcpdf']) || !$GLOBALS['_ext_pdf_tcpdf'])
{
	if(!class_exists('FPDF') && isset($GLOBALS['_ext_array']['FPDF']))
	{
//echo('require: '.$GLOBALS['_ext_array']['FPDF'].'<br />');
		require($GLOBALS['_ext_array']['FPDF']);
	}
	if(class_exists('FPDF'))
		$GLOBALS['_ext_pdf']='FPDF';
}
// ...TCPDF?
if(!strlen($GLOBALS['_ext_pdf']))
{
	if(!class_exists('TCPDF') && isset($GLOBALS['_ext_array']['TCPDF']))
	{
//echo('require: '.$GLOBALS['_ext_array']['TCPDF'].'<br />');
		require($GLOBALS['_ext_array']['TCPDF']);
	}
	if(class_exists('TCPDF'))
		$GLOBALS['_ext_pdf']='TCPDF';
}

// If we have PDF, we need our function collection
if(strlen($GLOBALS['_ext_pdf']) && !function_exists('fxpdf_mode'))
{
	$req_fn=fxf_fn_reqFilename('tools_pdf');
	if(strlen($req_fn))
	{
//echo('require: '.$req_fn.'<br />');
		require($req_fn);
	}

	// Not available?
	if(!function_exists('fxpdf_mode'))
		$GLOBALS['_ext_pdf']='';
}

// If we have PDF, do we also need our special graphical functions?
if(strlen($GLOBALS['_ext_pdf']) && isset($GLOBALS['_ext_pdf_pst']) && $GLOBALS['_ext_pdf_pst'])
{
	$req_fn=fxf_fn_reqFilename('tools_pdf_pst');
	if(strlen($req_fn))
	{
//echo('require: '.$req_fn.'<br />');
		require($req_fn);
	}
}

// FPDI available (needs FPDF)?
$GLOBALS['_ext_fpdi']='';
if(isset($GLOBALS['_ext_pdf_fpdi']) && $GLOBALS['_ext_pdf_fpdi'] && !class_exists('setasign\Fpdi\Fpdi') && isset($GLOBALS['_ext_array']['FPDI']))
{
	if(!class_exists('FPDF') && isset($GLOBALS['_ext_array']['FPDF']))
	{
//echo('require: '.$GLOBALS['_ext_array']['FPDF'].'<br />');
		require($GLOBALS['_ext_array']['FPDF']);
	}

	if(class_exists('FPDF'))
	{
		// Autoload
		$_fpdi_lsp=strrpos($GLOBALS['_ext_array']['FPDI'],'/');
		$_fpdi_autoload=substr($GLOBALS['_ext_array']['FPDI'],0,$_fpdi_lsp+1).'autoload.php';
//echo('require: '.$_fpdi_autoload.'<br />');
		if(file_exists($_fpdi_autoload))
			require($_fpdi_autoload);
	}
}
if(class_exists('setasign\Fpdi\Fpdi'))
	$GLOBALS['_ext_fpdi']='FPDI';

// Extend FPDF with added methods for fx-project
if(class_exists('FPDF') && !class_exists('xFPDF'))
{
	class xFPDF extends FPDF
	{
		// State Settings
		function SaveState()
		{
			$this->_out('q');
		}

		function RestoreState()
		{
			$this->_out('Q');
		}

		// Transformations
		function Translate($tx, $ty)
		{
			$this->_out('1 0 0 1 '.$tx.' '.$ty.' cm');
		}

		function Scale($sx, $sy)
		{
			$this->_out($sx.' 0 0 '.$sy.' 0 0 cm');
		}

		function Rotate($angle, $x=-1, $y=-1)
		{
			if($x == -1)
				$x=$this->x;
			if($y == -1)
				$y=$this->y;
			if($this->angle != 0)
				$this->_out('Q');
			$this->angle=$angle;
			if($angle != 0)
			{
				$angle *= M_PI/180;
				$c=cos($angle);
				$s=sin($angle);
				$cx=$x*$this->k;
				$cy=($this->h-$y)*$this->k;
				$this->_out(sprintf('q %.5f %.5f %.5f %.5f %.2f %.2f cm 1 0 0 1 %.2f %.2f cm', $c, $s, -$s, $c, $cx, $cy, -$cx, -$cy));
		    }
		}

		function Skew($deg_x, $deg_y)
		{
			$tanx=tan(deg2rad($deg_x));
			$tany=tan(deg2rad($deg_y));
			$this->_out('1 '.$tanx.' '.$tany.' 1 0 0 cm');
		}

		function SetLineStyle($lc=0, $lj=0)
		{
			$lc=min(2, max(0, (int)$lc));
			$lj=min(2, max(0, (int)$lj));
			// Set LineCap+LineJoin
			$this->_out($lc.' J '.$lj.' j');
		}

		function SetLineDash($bp='', $wp='')
		{
			// Set LineDash
			if(!$bp && !$wp)
				$this->_out('[] 0 d');
			else if(!$wp)
				$this->_out('['.$bp.'] 0 d');
			else
				$this->_out('['.$wp.' '.$bp.'] 0 d');
		}

		function MoveTo($x1, $y1)
		{
			// Set Starting Point
			$this->_out(sprintf('%.2f %.2f m', $x1*$this->k, ($this->h-$y1)*$this->k));
		}

		function LineTo($x1, $y1, $style='')
		{
			// Draw a line
			$this->_out(sprintf('%.2f %.2f l', $x1*$this->k, ($this->h-$y1)*$this->k));
			if($style)
			{
				$fill	= false;
				$stroke	= false;
				$op		= '';

				$style=strtoupper($style);
				if(strpos($style, 'C') !== false)
					$op='h ';
				if(strpos($style, 'F') !== false)
					$fill=true;
				if(strpos($style, 'S') !== false)
					$stroke=true;
				if(strpos($style, 'D') !== false)
					$stroke=true;

				if($fill && $stroke)
					$op .= 'B';
				else if($stroke)
					$op .= 'S';
				else
					$op .= 'f';
				$this->_out($op);
			}
		}
	}
}

// Extend TCPDF with added methods for fx-project
if(class_exists('TCPDF') && !class_exists('xTCPDF'))
{
	class xTCPDF extends TCPDF
	{
	}
}

// Delete all old temporary PDF files and unset PDF
if(strlen($GLOBALS['_ext_pdf']))
{
	fxDeleteTMP();

	if(($GLOBALS['fxpglobals']['tr'] != 84) && ($GLOBALS['fxpglobals']['tr'] != 226) && fxIsArray($GLOBALS['fxptdata']) && isset($GLOBALS['fxptdata']['pdf']))
		unset($GLOBALS['fxptdata']['pdf']);
}

//fxDebug(get_declared_classes(),'classes', 0);
?>