<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : pdf.inc                                                      //
// Version     : 24.1                                                         //
// Begin       : 2020-11-19                                                   //
// Last Change : 2025-02-12                                                   //
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
 * @version 24.1
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
	while(strlen((string)$_fpdf_path))
	{
		$_fpdf_lsp=strrpos($_fpdf_path,'/');
		if($_fpdf_lsp === false)
			break;
		$_fpdf_path=substr((string)$_fpdf_path,0,$_fpdf_lsp);
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
if(!strlen((string)$GLOBALS['_ext_pdf']))
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
if(strlen((string)$GLOBALS['_ext_pdf']) && !function_exists('fxpdf_mode'))
{
	$req_fn=fxf_fn_reqFilename('tools_pdf');
	if(strlen((string)$req_fn))
	{
//echo('require: '.$req_fn.'<br />');
		require($req_fn);
	}

	// Not available?
	if(!function_exists('fxpdf_mode'))
		$GLOBALS['_ext_pdf']='';
}

// If we have PDF, do we also need our special graphical functions?
if(strlen((string)$GLOBALS['_ext_pdf']) && isset($GLOBALS['_ext_pdf_pst']) && $GLOBALS['_ext_pdf_pst'])
{
	$req_fn=fxf_fn_reqFilename('tools_pdf_pst');
	if(strlen((string)$req_fn))
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
		$_fpdi_autoload=substr((string)$GLOBALS['_ext_array']['FPDI'],0,$_fpdi_lsp+1).'autoload.php';
//echo('require: '.$_fpdi_autoload.'<br />');
		if(file_exists($_fpdi_autoload))
			require($_fpdi_autoload);
	}
}
if(class_exists('setasign\Fpdi\Fpdi'))
	$GLOBALS['_ext_fpdi']='FPDI';

// Extend FPDF with added methods for fx-project and for PFD/A support with ZUGFeRD (Olivier, http://www.fpdf.org/en/script/index.php)
if(class_exists('FPDF') && !class_exists('xFPDF'))
{
	class xFPDF extends FPDF
	{
		// FeRox
		protected $fontfamily='';
		protected $pdfa=false;
		protected $facturx=false;
		protected $angle=0.0;
		protected $astates=array();

		// PFD/A
		protected $n_colorprofile;
		protected $n_metadata;

		// Attachment variables
		protected $files=array();
		protected $n_files;
		protected $open_attachment_pane=false;

		// ZUGFeRD variables
		protected $profile;
		protected $filename;


		// _____________________________________________________________________________________________________________
		// Methods added by FeRox (for compatibility with TCPDF)
		// =============================================================================================================
		// Font
		function SetFontFamily($family, $style='', $size=0)
		{
			$this->fontfamily=$family;
			$this->SetFont($family, $style, $size);
		}

		function GetFontFamily()
		{
			return $this->fontfamily;
		}

		// PDF/A
		function SetPDFA($b=true)
		{
			if($b)
				$this->pdfa=true;
			else
				$this->pdfa=false;
		}
		function GetPDFA()
		{
			return $this->pdfa;
		}

		// Factur-X
		function SetFacturX($b=true)
		{
			if($b)
			{
				$this->pdfa=true;
				$this->facturx=true;
			}
			else
				$this->facturx=false;
		}
		function GetFacturX()
		{
			return $this->facturx;
		}

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

				$style=strtoupper((string)$style);
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

		// alpha: Float value from 0 (transparent) to 1 (opaque)
		// bm:    Blend mode, one of the following: Normal, Multiply, Screen, Overlay, Darken, Lighten, ColorDodge, ColorBurn, HardLight, SoftLight, Difference, Exclusion, Hue, Saturation, Color, Luminosity
		function SetAlpha($alpha, $bm='Normal')
		{
			// Note: Alpha is not allowed in PDF/A
			if($this->pdfa)
				return;

			$n=$this->AddAState(array('ca'=>$alpha, 'CA'=>$alpha, 'BM'=>'/'.$bm, 'n'=>0));
			$this->SetAState($n);
		}

		function AddAState($a)
		{
			$this->_newobj();
			$a['n']=$this->n;

			$n=sizeof($this->astates);
			$this->astates[$n]=$a;

			return $n;
		}

		function SetAState($n)
	    {
			$ot=sprintf('/GS%d gs', $n);
			$this->_out($ot);
	    }

		function _putastates()
		{
			foreach($this->astates as $n => $a)
			{
				$this->_put('<</Type /ExtGState');
				$this->_put(sprintf('/ca %.3F', $a['ca']));
				$this->_put(sprintf('/CA %.3F', $a['CA']));
				$this->_put('/BM '.$a['BM']);
				$this->_put('>>');
				$this->_put('endobj');
			}
		}

		protected function _putresourcedict()
		{
			parent::_putresourcedict();
			// Alpha
			if(!empty($this->astates))
			{
				$this->_put('/ExtGState <<');
				foreach($this->astates as $n => $a)
				{
					$pt='/GS'.$n.' '.$a['n'].' 0 R';
					$this->_put($pt);
				}
				$this->_put('>>');
			}
		}

		// _____________________________________________________________________________________________________________
		// Methods added 2025-01-14 for PDF/A (from Olivier, http://www.fpdf.org)
		// =============================================================================================================
		protected function _putcolorprofile()
		{
			$icc = file_get_contents($GLOBALS['datpath'].'sRGB2014.icc');
			if(!$icc)
				$this->Error('Could not load the ICC profile');
			$this->_newobj();
			$this->n_colorprofile = $this->n;
			$this->_put('<<');
			$this->_put('/Length '.strlen($icc));
			$this->_put('/N 3');
			$this->_put('>>');
			$this->_putstream($icc);
			$this->_put('endobj');
		}

		protected function _getxmpdescription($prefix, $ns, $body)
		{
			return sprintf("\t<rdf:Description xmlns:%s=\"%s\" rdf:about=\"\">\n%s\t</rdf:Description>\n", $prefix, $ns, $body);
		}

		protected function _getxmpsimple($tag, $value)
		{
			$value = htmlspecialchars($value, ENT_XML1, 'UTF-8');
			return sprintf("\t\t<%s>%s</%s>\n", $tag, $value, $tag);
		}

		protected function _getxmpseq($tag, $value)
		{
			$value = htmlspecialchars($value, ENT_XML1, 'UTF-8');
			return sprintf("\t\t<%s>\n\t\t\t<rdf:Seq>\n\t\t\t\t<rdf:li>%s</rdf:li>\n\t\t\t</rdf:Seq>\n\t\t</%s>\n", $tag, $value, $tag);
		}

		protected function _getxmpalt($tag, $value)
		{
			$value = htmlspecialchars($value, ENT_XML1, 'UTF-8');
			return sprintf("\t\t<%s>\n\t\t\t<rdf:Alt>\n\t\t\t\t<rdf:li xml:lang=\"x-default\">%s</rdf:li>\n\t\t\t</rdf:Alt>\n\t\t</%s>\n", $tag, $value, $tag);
		}

		protected function _putmetadata()
		{
			$pdf = $this->_getxmpsimple('pdf:Producer', $this->metadata['Producer']);
			if(isset($this->metadata['Keywords']))
				$pdf .= $this->_getxmpsimple('pdf:Keywords', $this->metadata['Keywords']);

			$date = @date('c', $this->CreationDate);
			$xmp = $this->_getxmpsimple('xmp:CreateDate', $date);
			if(isset($this->metadata['Creator']))
				$xmp .= $this->_getxmpsimple('xmp:CreatorTool', $this->metadata['Creator']);

			$dc = '';
			if(isset($this->metadata['Author']))
				$dc .= $this->_getxmpseq('dc:creator', $this->metadata['Author']);
			if(isset($this->metadata['Title']))
				$dc .= $this->_getxmpalt('dc:title', $this->metadata['Title']);
			if(isset($this->metadata['Subject']))
				$dc .= $this->_getxmpalt('dc:description', $this->metadata['Subject']);

			$pdfaid = $this->_getxmpsimple('pdfaid:part', '3');
			$pdfaid .= $this->_getxmpsimple('pdfaid:conformance', 'B');

			$s  = '<?xpacket begin="" id="W5M0MpCehiHzreSzNTczkc9d"?>'."\n";
			$s .= '<x:xmpmeta xmlns:x="adobe:ns:meta/">'."\n";
			$s .= '<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#">'."\n";
			$s .= $this->_getxmpdescription('pdf', 'http://ns.adobe.com/pdf/1.3/', $pdf);
			$s .= $this->_getxmpdescription('xmp', 'http://ns.adobe.com/xap/1.0/', $xmp);
			if($dc)
				$s .= $this->_getxmpdescription('dc', 'http://purl.org/dc/elements/1.1/', $dc);
			$s .= $this->_getxmpdescription('pdfaid', 'http://www.aiim.org/pdfa/ns/id/', $pdfaid);
			$s .= $this->_getxmpextra();
			$s .= '</rdf:RDF>'."\n";
			$s .= '</x:xmpmeta>'."\n";
			$s .= '<?xpacket end="r"?>';

			$this->_newobj();
			$this->n_metadata = $this->n;
			$this->_put('<<');
			$this->_put('/Type /Metadata');
			$this->_put('/Subtype /XML');
			$this->_put('/Length '.strlen($s));
			$this->_put('>>');
			$this->_putstream($s);
			$this->_put('endobj');
		}

		protected function _putheader()
		{
			$this->_put('%PDF-1.7');
			$this->_put("%\xE2\xE3\xCF\xD3");
		}

		protected function _puttrailer()
		{
			parent::_puttrailer();
			$id = uniqid();
			$this->_put("/ID [($id)($id)]");
		}

		protected function _enddoc()
		{
			if($this->pdfa)
			{
				if(!isset($this->metadata['Producer']))
					$this->Error('Unsupported FPDF version');
//foreach($this->fonts as $font) fxDebug(array('name'=>$font['name'], 'type'=>$font['type']),'font: '.$font['path'], 0);
				foreach($this->fonts as $font)
				{
					if($font['type']=='Core')
						$this->Error('All fonts must be embedded in PDF/A');
				}
				$this->CreationDate = time();
			}
			parent::_enddoc();
		}


		// _____________________________________________________________________________________________________________
		// Methods added 2025-01-14 for Attachment (from Olivier, http://www.fpdf.org)
		// =============================================================================================================
		function Attach($file, $name='', $description='', $type='application/octet-stream', $compress=false, $relationship='Unspecified')
		{
			if($name=='')
			{
				$p = strrpos($file,'/');
				if($p===false)
					$p = strrpos($file,'\\');
				if($p!==false)
					$name = substr($file,$p+1);
				else
					$name = $file;
			}
			if($this->_isascii($name))
				$nameF = $name;
			else
				$nameF = 'file'.strrchr($name,'.');
			$this->files[] = array('file'=>$file, 'nameF'=>$nameF, 'nameUF'=>$name, 'desc'=>$description, 'type'=>$type, 'compress'=>$compress, 'rel'=>$relationship);
//fxDebug($this->files, 'Attach: '.$nameF, 0);
		}

		function OpenAttachmentPane()
		{
			$this->open_attachment_pane = true;
		}

		protected function _putfiles()
		{
			foreach($this->files as &$info)
			{
				$file = $info['file'];
				$desc = $info['desc'];
				$compress = $info['compress'] && $this->compress;

				$fc = file_get_contents($file);
				if($fc===false)
					$this->Error('Cannot open file: '.$file);
				$data = $compress ? gzcompress($fc) : $fc;
				$date = @date('YmdHisO', filemtime($file));
				$md = 'D:'.substr($date,0,-2)."'".substr($date,-2)."'";;

				$this->_newobj();
				$info['n'] = $this->n;
				$this->_put('<<');
				$this->_put('/Type /Filespec');
				$this->_put('/F '.$this->_textstring($info['nameF']));
				$this->_put('/UF '.$this->_textstring($info['nameUF']));
				$this->_put('/EF <</F '.($this->n+1).' 0 R /UF '.($this->n+1).' 0 R>>');
				if($desc)
					$this->_put('/Desc '.$this->_textstring($desc));
				$this->_put('/AFRelationship /'.$info['rel']);
				$this->_put('>>');
				$this->_put('endobj');

				$this->_newobj();
				$this->_put('<<');
				$this->_put('/Type /EmbeddedFile');
				$this->_put('/Subtype /'.str_replace('/', '#2F', $info['type']));
				if($compress)
					$this->_put('/Filter /FlateDecode');
				$this->_put('/Length '.strlen($data));
				$this->_put('/Params <</Size '.strlen($fc).' /ModDate '.$this->_textstring($md).'>>');
				$this->_put('>>');
				$this->_putstream($data);
				$this->_put('endobj');
			}
			unset($info);

			$this->_newobj();
			$this->n_files = $this->n;
			$a = array();
			foreach($this->files as $info)
				$a[] = $this->_textstring($info['nameF']).' '.$info['n'].' 0 R';
			$this->_put('<<');
			$this->_put('/Names ['.implode(' ',$a).']');
			$this->_put('>>');
			$this->_put('endobj');
		}

		protected function _putresources()
		{
			parent::_putresources();
			// PDF/A or Factur-X
			if($this->pdfa || $this->facturx)
			{
				$this->_putcolorprofile();
				$this->_putmetadata();
			}
			// Attachment
			if(!empty($this->files))
				$this->_putfiles();
			// Alpha
			if(!empty($this->astates))
				$this->_putastates();
		}

		protected function _putcatalog()
		{
			parent::_putcatalog();
			if($this->pdfa || $this->facturx)
			{
				$oi = '<</Type /OutputIntent /S /GTS_PDFA1 ';
				$oi .= '/OutputConditionIdentifier (sRGB2014.icc) /Info (sRGB2014.icc) /RegistryName (http://www.color.org) ';
				$oi .= '/DestOutputProfile '.$this->n_colorprofile.' 0 R>>';
				$this->_put('/OutputIntents ['.$oi.']');
				$this->_put('/Metadata '.$this->n_metadata.' 0 R');
			}
			if(!empty($this->files))
			{
				$this->_put('/Names <</EmbeddedFiles '.$this->n_files.' 0 R>>');
				$a = array();
				foreach($this->files as $info)
					$a[] = $info['n'].' 0 R';
				$this->_put('/AF ['.implode(' ',$a).']');
				if($this->open_attachment_pane)
					$this->_put('/PageMode /UseAttachments');
			}
		}


		// _____________________________________________________________________________________________________________
		// Methods added 2025-01-14 for ZUGFeRD (from Olivier, http://www.fpdf.org)
		// =============================================================================================================
		function SetXmlFile($file, $profile='EN 16931')
		{
			if(!in_array($profile, array('MINIMUM', 'BASIC WL', 'BASIC', 'EN 16931', 'EXTENDED', 'XRECHNUNG')))
				$this->Error('Incorrect profile: '.$profile);
			$this->profile = $profile;
			if($profile=='XRECHNUNG')
				$this->filename = 'xrechnung.xml';
			else
				$this->filename = 'factur-x.xml';
			if($profile=='MINIMUM' || $profile=='BASIC WL')
				$relationship = 'Data';
			else
				$relationship = 'Alternative';
			$this->Attach($file, $this->filename, '', 'text/xml', true, $relationship);
		}

		protected function _getxmpextra()
		{
			$extra = '';
			if($this->facturx)
			{
				if(empty($this->filename))
					$this->Error('The XML file has not been set');
				$fx = $this->_getxmpsimple('fx:DocumentType', 'INVOICE');
				$fx .= $this->_getxmpsimple('fx:DocumentFileName', $this->filename);
				$fx .= $this->_getxmpsimple('fx:Version', '1.0');
				$fx .= $this->_getxmpsimple('fx:ConformanceLevel', $this->profile);

				$extra = file_get_contents($GLOBALS['datpath'].'/xmp-extension.txt');
				if(!$extra)
					$this->Error('Could not load the XMP extension');
				$extra .= $this->_getxmpdescription('fx', 'urn:factur-x:pdfa:CrossIndustryDocument:invoice:1p0#', $fx);
			}
			return $extra;
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
if(strlen((string)$GLOBALS['_ext_pdf']))
{
	fxDeleteTMP();

	if(($GLOBALS['fxpglobals']['tr'] != 84) && ($GLOBALS['fxpglobals']['tr'] != 226) && fxIsArray($GLOBALS['fxptdata']) && isset($GLOBALS['fxptdata']['pdf']))
		unset($GLOBALS['fxptdata']['pdf']);
}

//fxDebug(get_declared_classes(),'classes', 0);
?>