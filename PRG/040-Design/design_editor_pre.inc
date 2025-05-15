<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : design_editor_pre.inc                                        //
// Version     : 24.1                                                         //
// Begin       : 2021-02-01                                                   //
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
 * PF 40: Design
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$req_fn=fxf_fn_reqFilename('design_functions', 40);
if(strlen((string)$req_fn))
	require($req_fn);

$lang=$GLOBALS['fxpglobals']['lang'];
//$lang=7;

// Design-Name
if(!isset($_POST['Button_Speichern']))
{
	if($mode == FXP_CREATE)
		$_POST['design']='';
	else
		$_POST['design']=$fxdsa[$dsg];
}

$d  = '<table border="0" cellpadding="0" cellspacing="0">'.$nl;
$d .= '<tr>'.$nl;

echo('<div id="dsg" style="display:none;">'.$dsg.'</div>');

// Create new temp stylesheet
echo('<div id="tcss_filename" style="display:none;">'.$tcss_filename.'</div>');
//echo('$tcss_filename='.$tcss_filename.'<br />');

$tcss='';
$dsa=false;
if(!isset($_POST['Button_Speichern']))
{
	$ucf=fxpDesignPath($dsg);
//echo('$dsg='.$dsg.', $ucf='.$ucf.'<br />');
   	if(strlen((string)$ucf))
   	{
		$tcss=fxpCSSDesign(true, $ucf);
//echo('<pre>'.fxHtmlEncode($tcss).'</pre><hr />');

		// Extract styles from design
		$dsa=fxpClientDesign($tcss,$ms,$lang);
//fxDebug($dsa,'$dsa', 0);
		// ...and create new version with all standard entries and save it
		$tcss=fxpCreateClientCSS($_POST['design'],$dsa,$ms);
//echo('<pre>'.fxHtmlEncode($tcss).'</pre><hr />');

		fxSave($tcss_filename, $tcss);
	}
}
else
	$tcss=fxLoad($tcss_filename);
//echo('<pre>'.fxHtmlEncode($tcss).'</pre><hr />');

// Extract styles from design
if(!is_array($dsa))
	$dsa=fxpClientDesign($tcss,$ms,$lang);
//fxDebug($dsa,'$dsa', 0);

// Mask width
$ew=(int)db_value("SELECT breite FROM masken WHERE nummer=15 and mandanten_id=0");

// Editor Mask
// ...Design-Name (design)
$maskenfeldwerte['design']=$_POST['design'];

// ...Basis (nv_text1)
if($mode == FXP_CREATE)
	$maskenfeldwerte['nv_text1']='~<font class="s5 lightgrey">&larr;</font>&nbsp;<font class="grey">'.$fxdsa[$dsg].'</font>';

// ...Feld (Feld)
$GLOBALS['Feld_ds_array']=array();
foreach($dsa as $did => $dar)
{
	if(substr((string)$did,0,5) != 'cmenu')	// Skip menu entries, they will be dealed with separately below
		$GLOBALS['Feld_ds_array'][$did]=$dar['name'];
}
if(!isset($_POST['Feld']))
	$_POST['Feld']='cfxbody';
$maskenfeldwerte['feld']=$_POST['Feld'];

$a=getDesignEditor($dsa,$_POST['Feld']);
//fxDebug($a,'$a', 0);

// ...Beschreibung (Beschreibung)
$maskenfeldwerte['beschreibung']='~<p id="de_dsc" class="grey" align="justify" style="margin:0;margin-bottom:8px;">'.$a['dsc'].'</p>';

// ...Einstellung (Einstellung)
$maskenfeldwerte['einstellung']='~<div id="de_set">'.$a['set'].'</div>';

// Menu editor mask
$a=getMenuDesignEditor($dsa,$msa,$ms,$lang);
// ...Textzeile -> Beschreibung (Textzeile)
$maskenfeldwerte['textzeile']='~<p id="mde_dsc" class="grey" align="justify" style="margin:0;margin-bottom:8px;">'.$a['dsc'].'</p>';
// ...Farbname -> Menüfarben (Farbname)
$maskenfeldwerte['farbname']='~<div id="mde_col">'.$a['col'].'</div>';
// ...Preview (bild)
$maskenfeldwerte['bild']='~<div id="mde_prv">'.$a['prv'].'</div>';

// Draw design and menu desdign editor mask
$d .= '<td valign="top" nowrap>'.$nl;
$d .= maske(15).$nl.'<br /><br >'.maske(17).$nl;
$d .= '</td>'.$nl;
$maske15_nv=true;
$maske17_nv=true;

// ...(hidden)
fieldHidden('mode',$mode);
fieldHidden('dsg',$dsg);
fieldHidden('msa',$msa);

// Preview
$pw=max(498,$GLOBALS['fxpglobals']['swidth']-$ew-140);
// ...Style
$s='<style id="preview_style_sheet" rel="stylesheet" type="text/css">'.$tcss.'</style>'.$nl;
//echo('<pre>'.fxHtmlEncode($s).'</pre>');
// ...Screen
$p=getDesignPreview($dsa,$msa);
//echo('<pre>'.fxHtmlEncode($p).'</pre>');
// ...Draw preview
$d .= '<td>&nbsp;</td>'.$nl;	// Spacer
$d .= '<td valign="top" nowrap>'.$nl;
$d .= std_mask(get_text(1898,'20'), $s.$p, $pw.'px');
$d .= '</td>'.$nl;

$d .= '</tr>'.$nl;
$d .= '</table>';

echo($d);
?>