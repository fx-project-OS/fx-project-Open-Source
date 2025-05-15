<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : ekvk_auswahl_pre.inc                                         //
// Version     : 24.1                                                         //
// Begin       : 2020-11-17                                                   //
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
 * 268: Hourly Rates
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$budgetart=strtoupper(substr((string)$GLOBALS['fxpglobals']['settings']['budgetart'], 0, 1)); // [N]=Keine Budgetart, [I]=Nur int. Budgetart, [E]=Nur ext. Budgetart + [B]=Beide Budgetarten
if($budgetart == 'N')
{
	// Auswahlmaske ausblenden...
	$maske520_nv=true;

	// ...und stattdessen Hinweis anzeigen
	// 2914: Dieser Mandant ist momentan so eingestellt, dass keine Budgetarten (intern oder extern) verwendet werden sollen, deshalb kann auch keine Stundensatzplanung durchgeführt werden!
	$msg='<div class="s4 red" style="border:1px solid #e1001a;padding:4px;border-radius:6px;background:#fff0f4;">'.meldung(2914, true).'</div>';
	if(tr_rechte(259))
	{
		// 2915: Um Stundensatzplanungen durchführen zu können aktivieren Sie bitte mindestens eine Budgetart (intern oder extern) in der Transaktion
		$msg .= '<br /><br /><br />'.meldung(2915, true).'<br /><br />'.fxpLink(259).'<br /><br />';
	}
	echo($msg);
}
else
{
	// Kategorie (Auswahl)
	$kat=0;
	if(isset($_POST))
		$kat=(int)$_POST['Kategorie'];
	if(!$kat)
		$kat=66;	// Vorgänge (als Default)

	$sql  = "SELECT f.id, t.literal_20";
	$sql .= " FROM felder f, texte t";
	$sql .= " WHERE t.id_referenz=f.id AND t.id_ref_art=3 AND t.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND f.id IN (21,66) AND f.mandanten_id=0";
	$sql .= " ORDER BY t.literal_20";

	$Kategorieselect		= $sql;
	$Kategorieselectwert	= 'f.id';
	$Kategorieselectfeld	= 'literal_20';
	$Kategorie_ds			= 0;

	$maskenfeldwerte['kategorie']=$kat;

	// Fähigkeiten (Auswahl)
	if(isset($_POST))
		$art=$_POST['MaArt_ID'];
	else
		$art=0;	// -leer- (als Default)

	$MaArt_ID_ds_array=array(-1=>'<i>['.meldung(388,false).']</i>', 0=>'<i>['.meldung(1422,false).']</i>', -2=>'<d>');	// -1=Ohne, 0=Alle
	$maskenfeldwerte['maart_id']=$art;

	// Auswahlfeld abhängig von Kategorie
	if(isset($_POST))
	{
		$id=$_POST['AuswahlFeldtyp'];
		$up=false;
		if($id !== NULL)
			$up=strpos($id, '_');
		if($up === false)
			$id=(int)$id;
	}
	else
		$id=0;	// -leer- (als Default)

	$maskenfeldwerte['auswahlfeldtyp']=$id;

	// ...Literal
	$Literal_20_lit='<font class="lightgrey">&gt;</font>&nbsp;<font id="catlit" class="lit">'.get_text($kat, '20').'</font>';

	// ...Auswahl
	$req_fn=fxf_fn_reqFilename('ekvk_select');
	if(strlen((string)$req_fn))
		require($req_fn);
}
?>