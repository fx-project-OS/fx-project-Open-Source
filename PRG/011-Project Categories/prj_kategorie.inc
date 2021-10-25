<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prj_kategorie.inc                                            //
// Version     : 21.2                                                         //
// Begin       : 2020-10-14                                                   //
// Last Change : 2020-10-14                                                   //
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
 * PF 11: Project Categories - Program called from JavaScript/AJAX
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

// Get additional project function collections
$GLOBALS['__includer']['tools_prj']=true;
require('includer.inc');

$cam=0;
if(fxIsArray($_GET) && isset($_GET['language']) && isset($_GET['gc']) && isset($_GET['cc']) && is_array($_POST) && isset($_POST['spos']) && isset($_POST['sval']))
{
	if(isset($_POST['mg']))
		$cam=1;
	else if(isset($_POST['sbg']))
		$cam=2;
}

if($cam > 0)
{
	// Übergabevariablen
	$language=(int)trim($_GET['language']);
	$gc=(int)trim($_GET['gc']);
	$cc=(int)trim($_GET['cc']);
	$spos=(int)$_POST['spos'];
	$sval=fxReplaceSpecialCharacters(trim($_POST['sval']));

	if($cam == 1)
		$mg=explode('~', $_POST['mg']);
	else
		$sbg=(int)$_POST['sbg'];

	// ...Close literal and Popup headline
	if($language == 1)
	{
		$close_lit='Schliesen';
		if($cam == 1)
			$headline='Projektkategorie ['.$spos.': &quot;'.$sval.'&quot;] verschieben nach...';
		else
		{
			$hg='';
			if($cc <= 0)
				$hg='gruppen';
			$headline='Kategorie'.$hg.'farbe ['.$spos.': &quot;'.$sval.'&quot;] &auml;ndern zu...';
		}
	}
	else
	{
		$close_lit='Close';
		if($cam == 1)
			$headline='Move Project Category ['.$spos.': &quot;'.$sval.'&quot;] to...';
		else
		{
			$hg='';
			if($cc <= 0)
				$hg=' Group';
			$headline='Change Category'.$hg.' Color ['.$spos.': &quot;'.$sval.'&quot;] to...';
		}
	}

	$ph=popupMask($headline, '', '', 'dcbpu', 640, 256, false, false, 0, 0, 'prjcatpc');
	echo($ph);

	// Main
	echo('<table width="100%" border="0" cellspacing="2" cellpadding="0">');
	if($cam == 1)
	{
		$lc=0;
		foreach($mg as $m)
		{
			$mt=explode('|', $m);

			echo('	<tr class="rv'.($rv%2).'" '.fxf_jsFunction('prjcatMoveIt', $gc, $cc, $mt[0]).' style="cursor:pointer;">');
			echo('		<td align="right" nowrap style="cursor:pointer;">&nbsp;<b>'.$mt[1].'</b>&nbsp;</td>');
			echo('		<td align="left" nowrap style="cursor:pointer;">&nbsp;&nbsp;'.fxReplaceSpecialCharacters($mt[2]).'&nbsp;</td>');
			echo('	</tr>');

			$lc++;
		}
	}
	else
		pk_backgrounds(2, $gc, $cc, $language);
//		pk_backgrounds(2, $gc.','.$cc, $language);
	echo('</table><br>');

	echo('</div>');
}
?>