/**
 * @license
 * Copyright 2012 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @fileoverview JavaScript for Blockly's Code demo.
 * @author fraser@google.com (Neil Fraser)
 */
'use strict';

/**
 * Create a namespace for the application.
 */
var Code = {};

/**
 * Lookup for names of supported languages.  Keys should be in ISO 639 format.
 */
Code.LANGUAGE_NAME = {
  'ar': 'العربية',
  'be-tarask': 'Taraškievica',
  'br': 'Brezhoneg',
  'ca': 'Català',
  'cs': 'Česky',
  'da': 'Dansk',
  'de': 'Deutsch',
  'el': 'Ελληνικά',
  'en': 'English',
  'es': 'Español',
  'et': 'Eesti',
  'fa': 'فارسی',
  'fr': 'Français',
  'he': 'עברית',
  'hrx': 'Hunsrik',
  'hu': 'Magyar',
  'ia': 'Interlingua',
  'is': 'Íslenska',
  'it': 'Italiano',
  'ja': '日本語',
  'kab': 'Kabyle',
  'ko': '한국어',
  'mk': 'Македонски',
  'ms': 'Bahasa Melayu',
  'nb': 'Norsk Bokmål',
  'nl': 'Nederlands, Vlaams',
  'oc': 'Lenga d\'òc',
  'pl': 'Polski',
  'pms': 'Piemontèis',
  'pt-br': 'Português Brasileiro',
  'ro': 'Română',
  'ru': 'Русский',
  'sc': 'Sardu',
  'sk': 'Slovenčina',
  'sr': 'Српски',
  'sv': 'Svenska',
  'ta': 'தமிழ்',
  'th': 'ภาษาไทย',
  'tlh': 'tlhIngan Hol',
  'tr': 'Türkçe',
  'uk': 'Українська',
  'vi': 'Tiếng Việt',
  'zh-hans': '简体中文',
  'zh-hant': '正體中文'
};

/**
 * List of RTL languages.
 */
Code.LANGUAGE_RTL = ['ar', 'fa', 'he', 'lki'];

/**
 * Blockly's main workspace.
 * @type {Blockly.WorkspaceSvg}
 */
Code.workspace = null;

/**
 * Extracts a parameter from the URL.
 * If the parameter is absent default_value is returned.
 * @param {string} name The name of the parameter.
 * @param {string} defaultValue Value to return if parameter not found.
 * @return {string} The parameter value or the default value if not found.
 */
Code.getStringParamFromUrl = function(name, defaultValue) {
  var val = location.search.match(new RegExp('[?&]' + name + '=([^&]+)'));
  return val ? decodeURIComponent(val[1].replace(/\+/g, '%20')) : defaultValue;
};

/**
 * Get the language of this user from the URL.
 * @return {string} User's language.
 */
Code.getLang = function() {
  var lang = Code.getStringParamFromUrl('lang', '');
  if (Code.LANGUAGE_NAME[lang] === undefined) {
    // Default to English.
    lang = 'en';
  }
  return lang;
};

/**
 * Is the current language (Code.LANG) an RTL language?
 * @return {boolean} True if RTL, false if LTR.
 */
Code.isRtl = function() {
  return Code.LANGUAGE_RTL.indexOf(Code.LANG) != -1;
};

/**
 * Load blocks saved on App Engine Storage or in session/local storage.
 * @param {string} defaultXml Text representation of default blocks.
 */
Code.loadBlocks = function(defaultXml) {
  try {
    var loadOnce = window.sessionStorage.loadOnceBlocks;
  } catch(e) {
    // Firefox sometimes throws a SecurityError when accessing sessionStorage.
    // Restarting Firefox fixes this, so it looks like a bug.
    var loadOnce = null;
  }
  if ('BlocklyStorage' in window && window.location.hash.length > 1) {
    // An href with #key trigers an AJAX call to retrieve saved blocks.
    BlocklyStorage.retrieveXml(window.location.hash.substring(1));
  } else if (loadOnce) {
    // Language switching stores the blocks during the reload.
    delete window.sessionStorage.loadOnceBlocks;
    var xml = Blockly.Xml.textToDom(loadOnce);
    Blockly.Xml.domToWorkspace(xml, Code.workspace);
  } else if (defaultXml) {
    // Load the editor with default starting blocks.
    var xml = Blockly.Xml.textToDom(defaultXml);
    Blockly.Xml.domToWorkspace(xml, Code.workspace);
  } else if ('BlocklyStorage' in window) {
    // Restore saved blocks in a separate thread so that subsequent
    // initialization is not affected from a failed load.
    window.setTimeout(BlocklyStorage.restoreBlocks, 0);
  }
};

/**
 * Save the blocks and reload with a different language.
 */
Code.changeLanguage = function() {
  // Store the blocks for the duration of the reload.
  // MSIE 11 does not support sessionStorage on file:// URLs.
  if (window.sessionStorage) {
    var xml = Blockly.Xml.workspaceToDom(Code.workspace);
    var text = Blockly.Xml.domToText(xml);
    window.sessionStorage.loadOnceBlocks = text;
  }

  var languageMenu = document.getElementById('languageMenu');
  var newLang = encodeURIComponent(
      languageMenu.options[languageMenu.selectedIndex].value);
  var search = window.location.search;
  if (search.length <= 1) {
    search = '?lang=' + newLang;
  } else if (search.match(/[?&]lang=[^&]*/)) {
    search = search.replace(/([?&]lang=)[^&]*/, '$1' + newLang);
  } else {
    search = search.replace(/\?/, '?lang=' + newLang + '&');
  }

  window.location = window.location.protocol + '//' +
      window.location.host + window.location.pathname + search;
};

/**
 * Changes the output language by clicking the tab matching
 * the selected language in the codeMenu.
 */
Code.changeCodingLanguage = function() {
  var codeMenu = document.getElementById('code_menu');
  Code.tabClick(codeMenu.options[codeMenu.selectedIndex].value);
}

/**
 * Bind a function to a button's click event.
 * On touch enabled browsers, ontouchend is treated as equivalent to onclick.
 * @param {!Element|string} el Button element or ID thereof.
 * @param {!Function} func Event handler to bind.
 */
Code.bindClick = function(el, func) {
  if (typeof el == 'string') {
    el = document.getElementById(el);
  }
  el.addEventListener('click', func, true);
  el.addEventListener('touchend', func, true);
};

/**
 * Load the Prettify CSS and JavaScript.
 */
Code.importPrettify = function() {
  var script = document.createElement('script');
  script.setAttribute('src', 'https://cdn.rawgit.com/google/code-prettify/master/loader/run_prettify.js');
  document.head.appendChild(script);
};

/**
 * Compute the absolute coordinates and dimensions of an HTML element.
 * @param {!Element} element Element to match.
 * @return {!Object} Contains height, width, x, and y properties.
 * @private
 */
Code.getBBox_ = function(element) {
  var height = element.offsetHeight;
  var width = element.offsetWidth;
  var x = 0;
  var y = 0;
  do {
    x += element.offsetLeft;
    y += element.offsetTop;
    element = element.offsetParent;
  } while (element);
  return {
    height: height,
    width: width,
    x: x,
    y: y
  };
};

/**
 * User's language (e.g. "en").
 * @type {string}
 */
Code.LANG = Code.getLang();

/**
 * List of tab names.
 * @private
 */
Code.TABS_ = ['blocks', 'lua', 'xml'];

/**
 * List of tab names with casing, for display in the UI.
 * @private
 */
Code.TABS_DISPLAY_ = [
  'Blocks', 'Lua', 'XML',
];

Code.selected = 'blocks';

/**
 * Switch the visible pane when a tab is clicked.
 * @param {string} clickedName Name of tab clicked.
 */
Code.tabClick = function(clickedName) {
  // If the XML tab was open, save and render the content.
  if (document.getElementById('tab_xml').classList.contains('tabon')) {
    var xmlTextarea = document.getElementById('content_xml');
    var xmlText = xmlTextarea.value;
    var xmlDom = null;
    try {
      xmlDom = Blockly.Xml.textToDom(xmlText);
    } catch (e) {
      var q =
          window.confirm(MSG['badXml'].replace('%1', e));
      if (!q) {
        // Leave the user on the XML tab.
        return;
      }
    }
    if (xmlDom) {
      Code.workspace.clear();
      Blockly.Xml.domToWorkspace(xmlDom, Code.workspace);
    }
  }

  if (document.getElementById('tab_blocks').classList.contains('tabon')) {
    Code.workspace.setVisible(false);
  }
  // Deselect all tabs and hide all panes.
  for (var i = 0; i < Code.TABS_.length; i++) {
    var name = Code.TABS_[i];
    var tab = document.getElementById('tab_' + name);
    tab.classList.add('taboff');
    tab.classList.remove('tabon');
    document.getElementById('content_' + name).style.visibility = 'hidden';
  }

  // Select the active tab.
  Code.selected = clickedName;
  var selectedTab = document.getElementById('tab_' + clickedName);
  selectedTab.classList.remove('taboff');
  selectedTab.classList.add('tabon');
  // Show the selected pane.
  document.getElementById('content_' + clickedName).style.visibility =
      'visible';
  Code.renderContent();
  // The code menu tab is on if the blocks tab is off.
  var codeMenuTab = document.getElementById('tab_code');
  if (clickedName == 'blocks') {
    Code.workspace.setVisible(true);
    codeMenuTab.className = 'taboff';
  } else {
    codeMenuTab.className = 'tabon';
  }
  // Sync the menu's value with the clicked tab value if needed.
  var codeMenu = document.getElementById('code_menu');
  for (var i = 0; i < codeMenu.options.length; i++) {
    if (codeMenu.options[i].value == clickedName) {
      codeMenu.selectedIndex = i;
      break;
    }
  }
  Blockly.svgResize(Code.workspace);
};

/**
 * Populate the currently selected pane with content generated from the blocks.
 */
Code.renderContent = function() {
  var content = document.getElementById('content_' + Code.selected);
  // Initialize the pane.
  if (content.id == 'content_xml') {
    var xmlTextarea = document.getElementById('content_xml');
    var xmlDom = Blockly.Xml.workspaceToDom(Code.workspace);
    var xmlText = Blockly.Xml.domToPrettyText(xmlDom);
    xmlTextarea.value = xmlText;
    xmlTextarea.focus();
  } else if (content.id == 'content_lua') {
    Code.attemptCodeGeneration(Blockly.Lua);
  }
  if (typeof PR == 'object') {
    PR.prettyPrint();
  }
};

/**
 * Attempt to generate the code and display it in the UI, pretty printed.
 * @param generator {!Blockly.Generator} The generator to use.
 */
Code.attemptCodeGeneration = function(generator) {
  var content = document.getElementById('content_' + Code.selected);
  content.textContent = '';
  if (Code.checkAllGeneratorFunctionsDefined(generator)) {
    var code = generator.workspaceToCode(Code.workspace);
    content.textContent = code;
    // Remove the 'prettyprinted' class, so that Prettify will recalculate.
    content.className = content.className.replace('prettyprinted', '');
  }
};

/**
 * Check whether all blocks in use have generator functions.
 * @param generator {!Blockly.Generator} The generator to use.
 */
Code.checkAllGeneratorFunctionsDefined = function(generator) {
  var blocks = Code.workspace.getAllBlocks(false);
  var missingBlockGenerators = [];
  for (var i = 0; i < blocks.length; i++) {
    var blockType = blocks[i].type;
    if (!generator[blockType]) {
      if (missingBlockGenerators.indexOf(blockType) == -1) {
        missingBlockGenerators.push(blockType);
      }
    }
  }

  var valid = missingBlockGenerators.length == 0;
  if (!valid) {
    var msg = 'The generator code for the following blocks not specified for ' +
        generator.name_ + ':\n - ' + missingBlockGenerators.join('\n - ');
    Blockly.alert(msg);  // Assuming synchronous. No callback.
  }
  return valid;
};

/**
 * Initialize Blockly.  Called on page load.
 */
Code.init = function() {
  Code.initLanguage();

  var rtl = Code.isRtl();
  var container = document.getElementById('content_area');
  var onresize = function(e) {
    var bBox = Code.getBBox_(container);
    for (var i = 0; i < Code.TABS_.length; i++) {
      var el = document.getElementById('content_' + Code.TABS_[i]);
      el.style.top = bBox.y + 'px';
      el.style.left = bBox.x + 'px';
      // Height and width need to be set, read back, then set again to
      // compensate for scrollbars.
      el.style.height = bBox.height + 'px';
      el.style.height = (2 * bBox.height - el.offsetHeight) + 'px';
      el.style.width = bBox.width + 'px';
      el.style.width = (2 * bBox.width - el.offsetWidth) + 'px';
    }
    // Make the 'Blocks' tab line up with the toolbox.
    if (Code.workspace && Code.workspace.getToolbox().width) {
      document.getElementById('tab_blocks').style.minWidth =
          (Code.workspace.getToolbox().width - 38) + 'px';
          // Account for the 19 pixel margin and on each side.
    }
  };
  window.addEventListener('resize', onresize, false);

  // The toolbox XML specifies each category name using Blockly's messaging
  // format (eg. `<category name="%{BKY_CATLOGIC}">`).
  // These message keys need to be defined in `Blockly.Msg` in order to
  // be decoded by the library. Therefore, we'll use the `MSG` dictionary that's
  // been defined for each language to import each category name message
  // into `Blockly.Msg`.
  // TODO: Clean up the message files so this is done explicitly instead of
  // through this for-loop.
  for (var messageKey in MSG) {
    if (messageKey.indexOf('cat') == 0) {
      Blockly.Msg[messageKey.toUpperCase()] = MSG[messageKey];
    }
  }

//===============================================================================
//  LIST
//===============================================================================

Blockly.Blocks['add_dict_element'] = {
  init: function() {
    this.appendDummyInput();
    this.appendDummyInput();
    this.appendValueInput("list")
        .setCheck(null)
        .appendField("in list:");
    this.appendValueInput("key")
        .setCheck(null)
        .appendField("set key:");
    this.appendValueInput("value")
        .setCheck(null)
        .appendField("with value:");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(260);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['add_dict_element'] = function(block) {

  var value_list = Blockly.Lua.valueToCode(block, 'list', Blockly.Lua.ORDER_ATOMIC);
  var value_key = Blockly.Lua.valueToCode(block, 'key', Blockly.Lua.ORDER_ATOMIC);
  var value_value = Blockly.Lua.valueToCode(block, 'value', Blockly.Lua.ORDER_ATOMIC);
  
  // TODO: Assemble Lua into code variable.
  var code = value_list+'['+value_key+']='+value_value+'\n';
  return code;
};

Blockly.Blocks['get_dict_element'] = {
  init: function() {
    this.appendValueInput("list")
        .setCheck(null)
        .appendField("in list:");
    this.appendValueInput("key")
        .setCheck(null)
        .appendField("get element with key:");
    this.setInputsInline(true);
    this.setOutput(true, null);
    this.setColour(260);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['get_dict_element'] = function(block) {
  var value_list = Blockly.Lua.valueToCode(block, 'list', Blockly.Lua.ORDER_ATOMIC);
  var value_key = Blockly.Lua.valueToCode(block, 'key', Blockly.Lua.ORDER_ATOMIC);


  var code = value_list+'['+value_key+']';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

/*Blockly.Blocks['require'] = {
  init: function() {
    this.appendValueInput("require")
        .setCheck("String")
        .appendField("require");
    this.setOutput(true, null);
    this.setColour(330);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['require'] = function(block) {
  var value_require = Blockly.Lua.valueToCode(block, 'require', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = '...';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};*/

//===============================================================================
//  OPENCV
//===============================================================================

Blockly.Blocks['opencv_img'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create image");
    this.setOutput(true, "opencv_img");
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_img'] = function(block) {
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.opencv.img()';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['opencv_videocapture'] = {
  init: function() {
    this.appendValueInput("from")
        .setCheck(["Number", "String"])
        .appendField("capture video from:");
    this.setOutput(true, "opencv_videocapture");
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_videocapture'] = function(block) {
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.opencv.videocapture('+value_from+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['opencv_setframesize'] = {
  init: function() {
    this.appendValueInput("NAME")
        .setCheck("opencv_videocapture")
        .appendField("set video capture");
    this.appendDummyInput()
        .appendField("with frame size")
        .appendField(new Blockly.FieldNumber(0,0), "w")
        .appendField(new Blockly.FieldNumber(0,0), "h");
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_setframesize'] = function(block) {
  var value_name = Blockly.Lua.valueToCode(block, 'NAME', Blockly.Lua.ORDER_ATOMIC);
  var number_w = block.getFieldValue('w');
  var number_h = block.getFieldValue('h');
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.opencv.setFrameSize('+value_name+','+number_w+','+number_h+')\n';
  return code;
};

Blockly.Blocks['opencv_setbuffersize'] = {
  init: function() {
    this.appendValueInput("NAME")
        .setCheck("opencv_videocapture")
        .appendField("set video capture");
    this.appendDummyInput()
        .appendField("with buffer size")
        .appendField(new Blockly.FieldNumber(0,0), "b")
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_setbuffersize'] = function(block) {
  var value_name = Blockly.Lua.valueToCode(block, 'NAME', Blockly.Lua.ORDER_ATOMIC);
  var number_b = block.getFieldValue('b');
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.opencv.setBufferSize('+value_name+','+number_b+')\n';
  return code;
};

Blockly.Blocks['opencv_frame'] = {
  init: function() {
    this.appendValueInput("from")
        .setCheck("opencv_videocapture")
        .appendField("get frame from");
    this.appendValueInput("to")
        .setCheck("opencv_img")
        .appendField("and put to");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_frame'] = function(block) {
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.opencv.frame('+value_from+','+value_to+')\n';
  return code;
};

Blockly.Blocks['opencv_setimg'] = {
  init: function() {
    this.appendValueInput("from")
        .setCheck("opencv_img")
        .appendField("set image");
    this.appendDummyInput()
        .appendField("w:")
        .appendField(new Blockly.FieldNumber(0, 0), "w")
        .appendField("h:")
        .appendField(new Blockly.FieldNumber(0, 0), "h")
        .appendField("color:")
        .appendField(new Blockly.FieldColour("#000000"), "color");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_setimg'] = function(block) {
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  var number_w = block.getFieldValue('w');
  var number_h = block.getFieldValue('h');
  var colour_color = block.getFieldValue('color');
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.opencv.setImg('+value_from+','+number_w+','+number_h+',"'+colour_color+'")\n';
  return code;
};

Blockly.Blocks['opencv_resize'] = {
  init: function() {
    this.appendValueInput("from")
        .setCheck("opencv_img")
        .appendField("resize image");
    this.appendValueInput("to")
        .setCheck("opencv_img")
        .appendField("and put to");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_resize'] = function(block) {
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.opencv.resize('+value_from+','+value_to+')\n';
  return code;
};

Blockly.Blocks['opencv_imencode'] = {
  init: function() {
    this.appendValueInput("from")
        .setCheck("opencv_img")
        .appendField("encode image");
    this.setInputsInline(false);
    this.setOutput(true, "String");
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_imencode'] = function(block) {
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.opencv.imencode('+value_from+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['opencv_imdecode'] = {
  init: function() {
    this.appendValueInput("from")
        .setCheck("String")
        .appendField("decode image");
    this.appendValueInput("to")
        .setCheck("opencv_img")
        .appendField("to");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_imdecode'] = function(block) {
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.opencv.imdecode('+value_from+','+value_to+')\n';
  return code;
};

//===============================================================================
//  GRAPHICS
//===============================================================================
Blockly.Blocks['graphics_newimage'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create new image with id:")
        .appendField(new Blockly.FieldTextInput("id"), "id")
        .appendField("from file:")
        .appendField(new Blockly.FieldTextInput("file"), "file")
        .appendField("x:")
        .appendField(new Blockly.FieldNumber(0, 0), "x")
        .appendField("y:")
        .appendField(new Blockly.FieldNumber(0, 0), "y")
        .appendField("w:")
        .appendField(new Blockly.FieldNumber(0, 0), "w")
        .appendField("h:")
        .appendField(new Blockly.FieldNumber(0, 0), "h");
    this.setInputsInline(true);
    this.setOutput(true, "graphics_object");
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['graphics_newimage'] = function(block) {
  var text_id = block.getFieldValue('id');
  var text_file = block.getFieldValue('file');
  var number_x = block.getFieldValue('x');
  var number_y = block.getFieldValue('y');
  var number_w = block.getFieldValue('w');
  var number_h = block.getFieldValue('h');
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.graphics.newImage("'+text_id+'","'+text_file+'",'+number_x+','+number_y+','+number_w+','+number_h+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['graphics_changeimage'] = {
  init: function() {
    this.appendValueInput("to")
        .setCheck("graphics_object")
        .appendField("change image");
    this.appendValueInput("from")
        .setCheck("opencv_img")
        .appendField("with");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['graphics_changeimage'] = function(block) {
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.graphics.changeImage('+value_to+','+value_from+')\n';
  return code;
};

//===============================================================================
//  LIB
//===============================================================================

Blockly.Blocks['form'] = {
  init: function() {
    //this.setStyle('list_blocks');
    this.setMutator(new Blockly.Mutator(['form_create_with_item']));
    this.itemCount_ = 3;
    
    this.appendValueInput("id")
      .setCheck("String")
      .appendField(new Blockly.FieldVariable("form"), "form")
      .appendField("create form with id:");

    this.appendValueInput("color")
      .setCheck(null)
      .setAlign(Blockly.ALIGN_RIGHT)
      .appendField("color:");

    this.updateShape_();

    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(330);
    this.setTooltip("");
    this.setHelpUrl("");
  },

 mutationToDom: function() {
    var container = Blockly.utils.xml.createElement('mutation');
    container.setAttribute('items', this.itemCount_);
    return container;
  },

  domToMutation: function(xmlElement) {
    this.itemCount_ = parseInt(xmlElement.getAttribute('items'), 10);
    this.updateShape_();
  },
decompose: function(workspace) {
    var containerBlock = workspace.newBlock('form_create_with_container');
    containerBlock.initSvg();
    var connection = containerBlock.getInput('STACK').connection;
    for (var i = 0; i < this.itemCount_; i++) {
      var itemBlock = workspace.newBlock('form_create_with_item');
      itemBlock.initSvg();
      connection.connect(itemBlock.previousConnection);
      connection = itemBlock.nextConnection;
    }
    return containerBlock;
  },

  compose: function(containerBlock) {
    var itemBlock = containerBlock.getInputTargetBlock('STACK');
    // Count number of inputs.
    var connections = [];
    while (itemBlock && !itemBlock.isInsertionMarker()) {
      connections.push(itemBlock.valueConnection_);
      itemBlock = itemBlock.nextConnection &&
          itemBlock.nextConnection.targetBlock();
    }
    // Disconnect any children that don't belong.
    for (var i = 0; i < this.itemCount_; i++) {
      var connection = this.getInput('ADD' + i).connection.targetConnection;
      if (connection && connections.indexOf(connection) == -1) {
        connection.disconnect();
      }
    }
    this.itemCount_ = connections.length;
    this.updateShape_();
    // Reconnect any child blocks.
    for (var i = 0; i < this.itemCount_; i++) {
      Blockly.Mutator.reconnect(connections[i], this, 'ADD' + i);
    }
  },
  updateShape_: function() {

    if (this.itemCount_ && this.getInput('EMPTY')) {
      this.removeInput('EMPTY');
    } else if (!this.itemCount_ && !this.getInput('EMPTY')) {
      this.appendDummyInput('EMPTY')
          .appendField("element");
    }

    // Add new inputs.
    for (var i = 0; i < this.itemCount_; i++) {
      if (!this.getInput('ADD' + i)) {
        if (i == 0) {
          //input.appendField("with");
          /*this.appendValueInput("id")
              .setCheck("String")
              .appendField("create form with id:");*/
        }
        var input = this.appendValueInput('ADD' + i)
            .setAlign(Blockly.ALIGN_RIGHT);
      }
    }
    // Remove deleted inputs.
    while (this.getInput('ADD' + i)) {
      this.removeInput('ADD' + i);
      i++;
    }

  }
};

Blockly.Blocks['form_create_with_container'] = {

  init: function() {
    //this.setStyle('list_blocks');
    this.appendDummyInput()
        .appendField("form");
    this.appendStatementInput('STACK');
    this.setTooltip("");
    this.contextMenu = false;
  }
};

Blockly.Blocks['form_create_with_item'] = {

  init: function() {
    //this.setStyle('list_blocks');
    this.appendDummyInput()
        .appendField("element");
    this.setPreviousStatement(true);
    this.setNextStatement(true);
    this.setTooltip("");
    this.contextMenu = false;
  }
};

Blockly.Lua['form'] = function(block) {
  var value_id = Blockly.Lua.valueToCode(block, 'id', Blockly.Lua.ORDER_ATOMIC);
  var variable_form = Blockly.Lua.variableDB_.getName(block.getFieldValue('form'), Blockly.Variables.NAME_TYPE);
  var value_color = Blockly.Lua.valueToCode(block, 'color', Blockly.Lua.ORDER_ATOMIC);

  var elements = new Array(block.itemCount_);
  for (var i = 0; i < block.itemCount_; i++) {
    elements[i] = Blockly.Lua.valueToCode(block, 'ADD' + i,
        Blockly.Lua.ORDER_NONE) || 'None';
  }

  var formClass = Blockly.Lua.provideFunction_(
    'Form',
    [ 'require "Form"']);

  var code = variable_form+"="+formClass+"("+value_id+")\n"+variable_form+".backgroundColor="+value_color+"\n"+variable_form+":addControl("+ elements.join(")\n"+variable_form+":addControl(") + ')\n';
  return code;
};

Blockly.Blocks['json'] = {
  init: function() {
    this.appendValueInput("encode")
        .setCheck(null)
        .appendField("json")
        .appendField(new Blockly.FieldDropdown([["encode","encode"], ["decode","decode"]]), "mode");
    this.setOutput(true, null);
    this.setColour(330);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['json'] = function(block) {
  var dropdown_mode = block.getFieldValue('mode');
  var value_encode = Blockly.Lua.valueToCode(block, 'encode', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.

  var jsonClass = Blockly.Lua.provideFunction_(
    'json',
    [ 'local ' + Blockly.Lua.FUNCTION_NAME_PLACEHOLDER_ + '=require "json"']);

  var code = jsonClass+"."+dropdown_mode+"("+value_encode+")";
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['show_form'] = {
  init: function() {
    this.appendValueInput("form")
        .setCheck(null)
        .appendField("show")

    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);

    this.setColour(330);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['show_form'] = function(block) {
  var value_form = Blockly.Lua.valueToCode(block, 'form', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.

  var code = value_form+":show()\n";
  // TODO: Change ORDER_NONE to the correct strength.
  return code;
};

//===============================================================================
//  SYSTEM
//===============================================================================

Blockly.Blocks['system_createtimer'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create a timer ");
    this.appendValueInput("time")
        .setCheck("Number")
        .appendField("activate every");
    this.appendValueInput("callback")
        .setCheck("timer_callback")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("with callback");
    this.setOutput(true, "timer");
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['system_createtimer'] = function(block) {
  var value_time = Blockly.Lua.valueToCode(block, 'time', Blockly.Lua.ORDER_ATOMIC);
  var value_callback = Blockly.Lua.valueToCode(block, 'callback', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = "proteo.system.createTimer("+value_time+","+value_callback+")";
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['system_callbacktimer'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("timer callback")
        .appendField(new Blockly.FieldVariable("dt"), "time");
    this.appendStatementInput("function")
        .setCheck(null);
    this.setOutput(true, "timer_callback");
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['system_callbacktimer'] = function(block) {
  var variable_time = Blockly.Lua.variableDB_.getName(block.getFieldValue('time'), Blockly.Variables.NAME_TYPE);
  var statements_function = Blockly.Lua.statementToCode(block, 'function');
  // TODO: Assemble Lua into code variable.
  var code = 'function('+variable_time+') \n'+statements_function+' end\n';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['system_clock'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("clock");
    this.setOutput(true, "String");
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['system_clock'] = function(block) {
  var code = 'proteo.system.clock()';
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['system_starttimer'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("start timer")
        .appendField(new Blockly.FieldVariable("timer"), "timer");
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['system_starttimer'] = function(block) {
  var variable_timer = Blockly.Lua.variableDB_.getName(block.getFieldValue('timer'), Blockly.Variables.NAME_TYPE);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.system.startTimer('+variable_timer+')\n';
  return code;
};

Blockly.Blocks['system_login'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("login with:");
    this.appendValueInput("user")
        .setCheck("String")
        .appendField("user");
    this.appendValueInput("password")
        .setCheck("String")
        .appendField("password");
    this.appendValueInput("script")
        .setCheck("String")
        .appendField("script");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['system_login'] = function(block) {
  var value_user = Blockly.Lua.valueToCode(block, 'user', Blockly.Lua.ORDER_ATOMIC);
  var value_password = Blockly.Lua.valueToCode(block, 'password', Blockly.Lua.ORDER_ATOMIC);
  var value_script = Blockly.Lua.valueToCode(block, 'script', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.system.login('+value_user+','+value_password+','+value_script+')\n';
  return code;
};

Blockly.Blocks['system_local'] = {
  init: function() {
    this.appendValueInput("to")
        .setCheck(null)
        .appendField("set local")
        .appendField(new Blockly.FieldVariable("item"), "NAME")
        .appendField("to");
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(330);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['system_local'] = function(block) {
  var variable_name = Blockly.Lua.variableDB_.getName(block.getFieldValue('NAME'), Blockly.Variables.NAME_TYPE);
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'local '+variable_name+'='+value_to+'\n';
  return code;
};
//===============================================================================
//  DEFAULT
//===============================================================================

Blockly.Blocks['init_function'] = {
  init: function() {
    this.appendStatementInput("init")
        .setCheck(null)
        .appendField("init");
    this.setColour(290);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};
Blockly.Lua['init_function'] = function(block) {
  var statements_init = Blockly.Lua.statementToCode(block, 'init');
  // TODO: Assemble Lua into code variable.
  var code = 'function init() \n'+statements_init+' end\n';
  return code;
};

//===============================================================================
//  ZMQ
//===============================================================================

Blockly.Blocks['zmq_createcontext'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create context");
    this.setOutput(true, "zmq_context");
    this.setColour(285);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['zmq_createcontext'] = function(block) {
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.zmq.ctx_new()';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['zmq_createsocket'] = {
  init: function() {
    this.appendValueInput("context")
        .setCheck("zmq_context")
        .appendField("create socket from");
    this.appendDummyInput()
        .appendField("of type")
        .appendField(new Blockly.FieldDropdown([["Request","ZMQ_REQ"], ["Reply","ZMQ_REP"], ["Pair","PAIR"]]), "type");
    this.setOutput(true, "zmq_socket");
    this.setColour(285);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['zmq_createsocket'] = function(block) {
  var value_context = Blockly.Lua.valueToCode(block, 'context', Blockly.Lua.ORDER_ATOMIC);
  var dropdown_type = block.getFieldValue('type');
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.zmq.socket_new('+value_context+',proteo.zmq.sockType.'+dropdown_type+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['zmq_bindconnect'] = {
  init: function() {
    this.appendValueInput("socket")
        .setCheck("zmq_socket")
        .appendField(new Blockly.FieldDropdown([["bind","bind"], ["connect","connect"]]), "operation")
        .appendField("socket");
    this.appendValueInput("url")
        .setCheck(null)
        .appendField("url");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(285);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['zmq_bindconnect'] = function(block) {
  var dropdown_operation = block.getFieldValue('operation');
  var value_socket = Blockly.Lua.valueToCode(block, 'socket', Blockly.Lua.ORDER_ATOMIC);
  var value_url = Blockly.Lua.valueToCode(block, 'url', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.zmq.'+dropdown_operation+'('+value_socket+','+value_url+')\n';
  return code;
};

Blockly.Blocks['zmq_send'] = {
  init: function() {
    this.appendValueInput("message")
        .setCheck("String")
        .appendField("send message");
    this.appendValueInput("to")
        .setCheck("zmq_socket")
        .appendField("to");
    this.appendDummyInput()
        .appendField(new Blockly.FieldDropdown([["","NULL"], ["dont wait","ZMQ_DONTWAIT"]]), "flag");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(285);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['zmq_send'] = function(block) {
  var value_message = Blockly.Lua.valueToCode(block, 'message', Blockly.Lua.ORDER_ATOMIC);
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  var dropdown_flag = block.getFieldValue('flag');
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.zmq.send ('+value_to+','+value_message+')\n';
  if(dropdown_flag!="NULL")
  {
    var code = 'proteo.zmq.send ('+value_to+','+value_message+',proteo.zmq.flag.'+dropdown_flag+')\n';
  }
  return code;
};

Blockly.Blocks['zmq_receive'] = {
  init: function() {
    this.appendValueInput("from")
        .setCheck("zmq_socket")
        .appendField("receive message from");
    this.appendDummyInput()
        .appendField(new Blockly.FieldDropdown([["","NULL"], ["dont wait","ZMQ_DONTWAIT"], ["send more","ZMQ_SNDMORE"]]), "flag");
    this.setOutput(true, null);
    this.setColour(285);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['zmq_receive'] = function(block) {
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  var dropdown_flag = block.getFieldValue('flag');
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.zmq.recv('+value_from+')';
  if(dropdown_flag!="NULL")
  {
    var code = 'proteo.zmq.recv('+value_from+',proteo.zmq.flag.'+dropdown_flag+')';
  }
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

//===============================================================================
//  ENET
//===============================================================================

Blockly.Blocks['enet_hostcreate'] = {
  init: function() {
    this.appendValueInput("host")
        .setCheck("String")
        .appendField("create host");
    this.setOutput(true, "enet_host");
    this.setColour(290);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['enet_hostcreate'] = function(block) {

  var value_host = Blockly.Lua.valueToCode(block, 'host', Blockly.Lua.ORDER_ATOMIC);
  
  var code = 'proteo.enet.host_create('+value_host+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['enet_connect'] = {
  init: function() {
    this.appendValueInput("host")
        .setCheck("enet_host")
        .appendField("connect host");
    this.appendValueInput("to")
        .setCheck("String")
        .appendField("to");
    this.setInputsInline(true);
    this.setOutput(true, "enet_peer");
    this.setColour(290);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['enet_connect'] = function(block) {
  var value_host = Blockly.Lua.valueToCode(block, 'host', Blockly.Lua.ORDER_ATOMIC);
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = value_host+':connect('+value_to+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['enet_service'] = {
  init: function() {
    this.appendDummyInput()
        .appendField(new Blockly.FieldVariable("host"), "host")
        .appendField("service with timeout")
        .appendField(new Blockly.FieldNumber(0, 0), "timeout");
    this.setInputsInline(true);
    this.setOutput(true, "enet_event");
    this.setColour(290);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['enet_service'] = function(block) {
  var variable_host = Blockly.Lua.variableDB_.getName(block.getFieldValue('host'), Blockly.Variables.NAME_TYPE);
  var number_timeout = block.getFieldValue('timeout');
  // TODO: Assemble Lua into code variable.
  var code = variable_host+':service('+number_timeout+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['enet_handleevent'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("handle")
        .appendField(new Blockly.FieldVariable("event"), "event");
    this.appendStatementInput("connect")
        .setCheck(null)
        .appendField("connect");
    this.appendStatementInput("receive")
        .setCheck(null)
        .appendField("receive");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(290);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['enet_handleevent'] = function(block) {
  var variable_event = Blockly.Lua.variableDB_.getName(block.getFieldValue('event'), Blockly.Variables.NAME_TYPE);
  var statements_connect = Blockly.Lua.statementToCode(block, 'connect');
  var statements_receive = Blockly.Lua.statementToCode(block, 'receive');
  // TODO: Assemble Lua into code variable.
  var code = 'if '+variable_event+' then \n if '+variable_event+'.type == "connect" then\n' +statements_connect+'\n elseif '+variable_event+'.type == "receive" then\n' +statements_receive+ '\n end\nend \n';
  return code;
};

Blockly.Blocks['enet_peer'] = {
  init: function() {
    this.appendDummyInput()
        .appendField(new Blockly.FieldVariable("event"), "event")
        .appendField("peer");
    this.setInputsInline(true);
    this.setOutput(true, "enet_peer");
    this.setColour(290);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['enet_peer'] = function(block) {
  var variable_event = Blockly.Lua.variableDB_.getName(block.getFieldValue('event'), Blockly.Variables.NAME_TYPE);
  // TODO: Assemble Lua into code variable.
  var code = variable_event+'.peer';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['enet_data'] = {
  init: function() {
    this.appendDummyInput()
        .appendField(new Blockly.FieldVariable("event"), "event")
        .appendField("data");
    this.setInputsInline(true);
    this.setOutput(true, null);
    this.setColour(290);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['enet_data'] = function(block) {
  var variable_event = Blockly.Lua.variableDB_.getName(block.getFieldValue('event'), Blockly.Variables.NAME_TYPE);
  // TODO: Assemble Lua into code variable.
  var code = variable_event+'.data';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['enet_send'] = {
  init: function() {
    this.appendValueInput("data")
        .setCheck("String")
        .appendField("send");
    this.appendValueInput("to")
        .setCheck("enet_peer")
        .appendField("to");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(290);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['enet_send'] = function(block) {
  var value_data = Blockly.Lua.valueToCode(block, 'data', Blockly.Lua.ORDER_ATOMIC);
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = value_to+':send('+value_data+')\n';
  return code;
};

//===============================================================================
//  NETWORK
//===============================================================================

Blockly.Blocks['network_port'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("PORT");
    this.setOutput(true, "String");
    this.setColour(210);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['network_port'] = function(block) {
  // TODO: Assemble Lua into code variable.
  var code = 'PORT';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['network_baseurl'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("BASEURL");
    this.setOutput(true, "String");
    this.setColour(210);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['network_baseurl'] = function(block) {
  var code = 'BASEURL';
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['network_proteopost'] = {
  init: function() {
    this.appendValueInput("data")
        .setCheck("String")
        .appendField("proteo POST")
        .appendField(new Blockly.FieldTextInput("/"), "url")
        .appendField("data");
    this.appendValueInput("call")
        .setCheck("network_callback")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("callback");
    this.setInputsInline(false);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(210);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['network_proteopost'] = function(block) {
  var text_url = block.getFieldValue('url');
  var value_data = Blockly.Lua.valueToCode(block, 'data', Blockly.Lua.ORDER_ATOMIC);
  var value_call = Blockly.Lua.valueToCode(block, 'call', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.network.proteo_post("'+text_url+'",'+value_data+','+value_call+')\n';
  return code;
};

Blockly.Blocks['network_callback'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("network callback")
        .appendField(new Blockly.FieldVariable("res"), "res")
        .appendField(new Blockly.FieldVariable("data"), "data");
    this.appendStatementInput("function")
        .setCheck(null);
    this.setOutput(true, "network_callback");
    this.setColour(210);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['network_callback'] = function(block) {
  var variable_res = Blockly.Lua.variableDB_.getName(block.getFieldValue('res'), Blockly.Variables.NAME_TYPE);
  var variable_data = Blockly.Lua.variableDB_.getName(block.getFieldValue('data'), Blockly.Variables.NAME_TYPE);
  var statements_function = Blockly.Lua.statementToCode(block, 'function');
  // TODO: Assemble Lua into code variable.
  var code = 'function('+variable_res+','+variable_data+') \n'+statements_function+' end\n';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

//===============================================================================
//  ROUTE
//===============================================================================

Blockly.Blocks['get_route'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("GET path")
        .appendField(new Blockly.FieldTextInput("/"), "path");
    this.appendStatementInput("function")
        .setCheck(null);
    this.appendValueInput("return")
        .setCheck("String")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("return");
    this.setColour(330);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['get_route'] = function(block) {
  var text_path = block.getFieldValue('path');
  var statements_function = Blockly.Lua.statementToCode(block, 'function');
  var value_return = Blockly.Lua.valueToCode(block, 'return', Blockly.Lua.ORDER_ATOMIC);
  

  var code = 'proteo.route.get("'+text_path+'",\n function(username,permission,data,param)\n'+statements_function+'\n return '+value_return+'\nend\n)\n';

  return code;
};

Blockly.Blocks['post_route'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("POST path")
        .appendField(new Blockly.FieldTextInput("/"), "path");
    this.appendStatementInput("function")
        .setCheck(null);
    this.appendValueInput("return")
        .setCheck("String")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("return");
    this.setColour(330);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['post_route'] = function(block) {
  var text_path = block.getFieldValue('path');
  var statements_function = Blockly.Lua.statementToCode(block, 'function');
  var value_return = Blockly.Lua.valueToCode(block, 'return', Blockly.Lua.ORDER_ATOMIC);
  
  var code = 'proteo.route.post("'+text_path+'",\n function(username,permission,data,param)\n'+statements_function+'\n return '+value_return+'\nend\n)\n';

  return code;
};

/*Blockly.Lua['get_route'] = function(block) {
  var text_path = block.getFieldValue('path');
  var statements_function = Blockly.Lua.statementToCode(block, 'function');
  var dropdown_decode = block.getFieldValue('decode');
  var value_return = Blockly.Lua.valueToCode(block, 'return', Blockly.Lua.ORDER_ATOMIC);
  
  var ret=value_return;
  if (dropdown_decode=="return_json")
  {
    ret="json.encode("+value_return+")";
  }

  var code = 'proteo.route.get("'+text_path+'",\n function(username,permission,data,param)\n'+statements_function+'\nreturn '+ret+'\nend\n)\n';

  return code;
};*/

//===============================================================================
//  GUI
//===============================================================================

Blockly.Blocks['gui_minx'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("MIN_X");
    this.setOutput(true, "Number");
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_minx'] = function(block) {
  // TODO: Assemble Lua into code variable.
  var code = 'MIN_X';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['gui_midx'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("MID_X");
    this.setOutput(true, "Number");
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_midx'] = function(block) {
  // TODO: Assemble Lua into code variable.
  var code = 'MID_X';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['gui_maxx'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("MAX_X");
    this.setOutput(true, "Number");
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_maxx'] = function(block) {
  // TODO: Assemble Lua into code variable.
  var code = 'MAX_X';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['gui_miny'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("MIN_Y");
    this.setOutput(true, "Number");
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_miny'] = function(block) {
  // TODO: Assemble Lua into code variable.
  var code = 'MIN_Y';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['gui_midy'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("MID_Y");
    this.setOutput(true, "Number");
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_midy'] = function(block) {
  // TODO: Assemble Lua into code variable.
  var code = 'MID_Y';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['gui_maxy'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("MAX_Y");
    this.setOutput(true, "Number");
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_maxy'] = function(block) {
  // TODO: Assemble Lua into code variable.
  var code = 'MAX_Y';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['gui_font'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("font");
    this.appendValueInput("name")
        .setCheck("String")
        .appendField("name:");
    this.appendValueInput("size")
        .setCheck("Number")
        .appendField("size:");
    this.appendValueInput("color")
        .setCheck(null)
        .appendField("color:");
    this.setInputsInline(true);
    this.setOutput(true, "gui_font");
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_font'] = function(block) {
  var value_name = Blockly.Lua.valueToCode(block, 'name', Blockly.Lua.ORDER_ATOMIC);
  var value_size = Blockly.Lua.valueToCode(block, 'size', Blockly.Lua.ORDER_ATOMIC);
  var value_color = Blockly.Lua.valueToCode(block, 'color', Blockly.Lua.ORDER_ATOMIC);

  var code =value_name+','+value_size+','+value_color;

  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['gui_position'] = {
  init: function() {
    this.appendValueInput("x")
        .setCheck("Number")
        .appendField("X:");
    this.appendValueInput("y")
        .setCheck("Number")
        .appendField("Y:");
    this.appendValueInput("w")
        .setCheck("Number")
        .appendField("W:");
    this.appendValueInput("h")
        .setCheck("Number")
        .appendField("H:");
    this.setInputsInline(true);
    this.setOutput(true, "gui_position");
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_position'] = function(block) {
  var value_x = Blockly.Lua.valueToCode(block, 'x', Blockly.Lua.ORDER_ATOMIC);
  var value_y = Blockly.Lua.valueToCode(block, 'y', Blockly.Lua.ORDER_ATOMIC);
  var value_w = Blockly.Lua.valueToCode(block, 'w', Blockly.Lua.ORDER_ATOMIC);
  var value_h = Blockly.Lua.valueToCode(block, 'h', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = value_x+','+value_y+','+value_w+','+value_h;
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['gui_newlabel'] = {
  init: function() {
    this.appendValueInput("id")
        .setCheck("String")
        .appendField("create new label with")
        .appendField("id:");
    this.appendValueInput("text")
        .setCheck("String")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("text:");
    this.appendValueInput("font")
        .setCheck("gui_font")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("font:");
    this.appendValueInput("back")
        .setCheck(null) //["colour_rgb", "colour_picker"])
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("back color:");
    this.appendDummyInput()
        .appendField("align")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField(new Blockly.FieldDropdown([["left","LEFT"], ["center","CENTER"], ["right","RIGHT"]]), "align");
    this.appendValueInput("position")
        .setCheck("gui_position")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("position:");
    this.setInputsInline(false);
    this.setOutput(true, "gui_object");
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_newlabel'] = function(block) {
  var value_id = Blockly.Lua.valueToCode(block, 'id', Blockly.Lua.ORDER_ATOMIC);
  var value_text = Blockly.Lua.valueToCode(block, 'text', Blockly.Lua.ORDER_ATOMIC);
  var value_font = Blockly.Lua.valueToCode(block, 'font', Blockly.Lua.ORDER_ATOMIC);
  var value_back = Blockly.Lua.valueToCode(block, 'back', Blockly.Lua.ORDER_ATOMIC);
  var dropdown_align = block.getFieldValue('align');
  var value_position = Blockly.Lua.valueToCode(block, 'position', Blockly.Lua.ORDER_ATOMIC);
  
  var align="proteo.gui.LabelAlignment.Right";
  if (dropdown_align=="CENTER")
  {
    align="proteo.gui.LabelAlignment.Center";
  }else if(dropdown_align=="LEFT")
  {
    align="proteo.gui.LabelAlignment.Left";
  }

  var code = 'proteo.gui.newLabel('+value_id+','+value_text+','+value_font+','+value_back+','+align+','+value_position+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['gui_newsimpletextfield'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create new text field with id:")
        .appendField(new Blockly.FieldTextInput("id"), "id")
        .appendField("text:")
        .appendField(new Blockly.FieldTextInput("text"), "text")
        .appendField("font:")
        .appendField(new Blockly.FieldDropdown([["Colaborate","Colaborate"], ["Helvetica","Helvetica"], ["OpenSans","OpenSans"]]), "font")
        .appendField(new Blockly.FieldNumber(0), "fontsize")
        .appendField(new Blockly.FieldColour("#000000"), "fontcolor")
        .appendField("back color:")
        .appendField(new Blockly.FieldColour("#99ff99"), "back")
        .appendField("x:")
        .appendField(new Blockly.FieldNumber(0), "x")
        .appendField("y:")
        .appendField(new Blockly.FieldNumber(0), "y")
        .appendField("w:")
        .appendField(new Blockly.FieldNumber(0), "w")
        .appendField("h:")
        .appendField(new Blockly.FieldNumber(0), "h");
    this.appendValueInput("callback")
        .setCheck("gui_callback")
        .appendField("call:");
    this.setInputsInline(true);
    this.setOutput(true, "gui_object");
    this.setColour(120);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_newsimpletextfield'] = function(block) {
  var text_id = block.getFieldValue('id');
  var text_text = block.getFieldValue('text');
  var dropdown_font = block.getFieldValue('font');
  var number_fontsize = block.getFieldValue('fontsize');
  var colour_fontcolor = block.getFieldValue('fontcolor');
  var colour_back = block.getFieldValue('back');
  var number_x = block.getFieldValue('x');
  var number_y = block.getFieldValue('y');
  var number_w = block.getFieldValue('w');
  var number_h = block.getFieldValue('h');
  var value_callback = Blockly.Lua.valueToCode(block, 'callback', Blockly.Lua.ORDER_ATOMIC);

var callback='""';
  if (value_callback!="")
    callback=value_callback;
  
   var code = 'proteo.gui.newTextField("'+text_id+'","'+text_text+'","'+dropdown_font+'",'+number_fontsize+',"'+colour_fontcolor+'","'+colour_back+'",'+number_x+','+number_y+','+number_w+','+number_h+','+callback+')';

  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['gui_newsimplebutton'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create new button with id:")
        .appendField(new Blockly.FieldTextInput("id"), "id")
        .appendField("text:")
        .appendField(new Blockly.FieldTextInput("text"), "text")
        .appendField("font:")
        .appendField(new Blockly.FieldDropdown([["Colaborate","Colaborate"], ["Helvetica","Helvetica"], ["OpenSans","OpenSans"]]), "font")
        .appendField(new Blockly.FieldNumber(0), "fontsize")
        .appendField(new Blockly.FieldColour("#000000"), "fontcolor")
        .appendField("back color:")
        .appendField(new Blockly.FieldColour("#99ff99"), "back")
        .appendField("border size:")
        .appendField(new Blockly.FieldNumber(0), "border")
        .appendField("color:")
        .appendField(new Blockly.FieldColour("#006600"), "color")
        .appendField(new Blockly.FieldDropdown([["squared","squared"], ["rounded","rounded"]]), "squared")
        .appendField("x:")
        .appendField(new Blockly.FieldNumber(0), "x")
        .appendField("y:")
        .appendField(new Blockly.FieldNumber(0), "y")
        .appendField("w:")
        .appendField(new Blockly.FieldNumber(0), "w")
        .appendField("h:")
        .appendField(new Blockly.FieldNumber(0), "h");
    this.appendValueInput("callback")
        .setCheck("gui_callback")
        .appendField("call:");
    this.setInputsInline(true);
    this.setOutput(true, "gui_object");
    this.setColour(120);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_newsimplebutton'] = function(block) {
  var text_id = block.getFieldValue('id');
  var text_text = block.getFieldValue('text');
  var dropdown_font = block.getFieldValue('font');
  var number_fontsize = block.getFieldValue('fontsize');
  var colour_fontcolor = block.getFieldValue('fontcolor');
  var colour_back = block.getFieldValue('back');
  var number_border = block.getFieldValue('border');
  var colour_color = block.getFieldValue('color');
  var dropdown_squared = block.getFieldValue('squared');
  var number_x = block.getFieldValue('x');
  var number_y = block.getFieldValue('y');
  var number_w = block.getFieldValue('w');
  var number_h = block.getFieldValue('h');
  var value_callback = Blockly.Lua.valueToCode(block, 'callback', Blockly.Lua.ORDER_ATOMIC);

var rounded=true;
if(dropdown_squared!="rounded") rounded=false;

var callback='""';
  if (value_callback!="")
    callback=value_callback;

   var code = 'proteo.gui.newButton("'+text_id+'","'+text_text+'","'+dropdown_font+'",'+number_fontsize+',"'+colour_fontcolor+'","'+colour_back+'",'+number_border+',"'+colour_color+'",'+rounded+','+number_x+','+number_y+','+number_w+','+number_h+','+callback+')';

  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['gui_newbutton'] = {
  init: function() {
    this.appendValueInput("id")
        .setCheck("String")
        .appendField("create new button with")
        .appendField("id:");
    this.appendValueInput("text")
        .setCheck("String")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("text:");
    this.appendValueInput("font")
        .setCheck("gui_font")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("font:");
    this.appendValueInput("back")
        .setCheck(null)
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("back color:");
    this.appendValueInput("border")
        .setCheck("Number")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("border size:");
    this.appendValueInput("color")
        .setCheck(null)
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("border color:");
    this.appendDummyInput()
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField(new Blockly.FieldDropdown([["squared","OPTIONNAME"], ["rounded","OPTIONNAME"]]), "NAME");
    this.appendValueInput("position")
        .setCheck("gui_position")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("position:");
    this.appendValueInput("NAME")
        .setCheck("gui_callback")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("callback:");
    this.setInputsInline(false);
    this.setOutput(true, "gui_object");
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_newbutton'] = function(block) {
  var value_id = Blockly.Lua.valueToCode(block, 'id', Blockly.Lua.ORDER_ATOMIC);
  var value_text = Blockly.Lua.valueToCode(block, 'text', Blockly.Lua.ORDER_ATOMIC);
  var value_font = Blockly.Lua.valueToCode(block, 'font', Blockly.Lua.ORDER_ATOMIC);
  var value_back = Blockly.Lua.valueToCode(block, 'back', Blockly.Lua.ORDER_ATOMIC);
  var value_border = Blockly.Lua.valueToCode(block, 'border', Blockly.Lua.ORDER_ATOMIC);
  var value_color = Blockly.Lua.valueToCode(block, 'color', Blockly.Lua.ORDER_ATOMIC);
  var dropdown_name = block.getFieldValue('NAME');
  var value_position = Blockly.Lua.valueToCode(block, 'position', Blockly.Lua.ORDER_ATOMIC);
  var value_name = Blockly.Lua.valueToCode(block, 'NAME', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = '...';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['gui_callback'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("gui callback")
        .appendField(new Blockly.FieldVariable("sender"), "sender");
    this.appendStatementInput("function")
        .setCheck(null);
    this.setOutput(true, "gui_callback");
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_callback'] = function(block) {
  var variable_sender = Blockly.Lua.variableDB_.getName(block.getFieldValue('sender'), Blockly.Variables.NAME_TYPE);
  var statements_function = Blockly.Lua.statementToCode(block, 'function');
  // TODO: Assemble Lua into code variable.
  var code = 'function('+variable_sender+') \n'+statements_function+' end\n';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['gui_newsimplelabel'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create new label with id:")
        .appendField(new Blockly.FieldTextInput("id"), "id")
        .appendField("text:")
        .appendField(new Blockly.FieldTextInput("text"), "text")
        .appendField("font:")
        .appendField(new Blockly.FieldDropdown([["Colaborate","Colaborate"], ["Helvetica","Helvetica"], ["OpenSans","OpenSans"]]), "font")
        .appendField(new Blockly.FieldNumber(0), "fontsize")
        .appendField(new Blockly.FieldColour("#000000"), "fontcolor")
        .appendField("back color:")
        .appendField(new Blockly.FieldColour("#99ff99"), "back")
        .appendField(new Blockly.FieldDropdown([["left","LEFT"], ["center","CENTER"], ["right","RIGHT"]]), "align")
        .appendField("x:")
        .appendField(new Blockly.FieldNumber(0), "x")
        .appendField("y:")
        .appendField(new Blockly.FieldNumber(0), "y")
        .appendField("w:")
        .appendField(new Blockly.FieldNumber(0), "w")
        .appendField("h:")
        .appendField(new Blockly.FieldNumber(0), "h");
    this.setInputsInline(true);
    this.setOutput(true, "gui_object");
    this.setColour(120);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_newsimplelabel'] = function(block) {
  var text_id = block.getFieldValue('id');
  var text_text = block.getFieldValue('text');
  var dropdown_font = block.getFieldValue('font');
  var number_fontsize = block.getFieldValue('fontsize');
  var colour_fontcolor = block.getFieldValue('fontcolor');
  var colour_back = block.getFieldValue('back');
  var dropdown_align = block.getFieldValue('align');
  var number_x = block.getFieldValue('x');
  var number_y = block.getFieldValue('y');
  var number_w = block.getFieldValue('w');
  var number_h = block.getFieldValue('h');
  
  var align="proteo.gui.LabelAlignment.Right";
  if (dropdown_align=="CENTER")
  {
    align="proteo.gui.LabelAlignment.Center";
  }else if(dropdown_align=="LEFT")
  {
    align="proteo.gui.LabelAlignment.Left";
  }

  var code = 'proteo.gui.newLabel("'+text_id+'","'+text_text+'","'+dropdown_font+'",'+number_fontsize+',"'+colour_fontcolor+'","'+colour_back+'",'+align+','+number_x+','+number_y+','+number_w+','+number_h+')';

  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['gui_newsimpledropdown'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create new dropbox with id:")
        .appendField(new Blockly.FieldTextInput("id"), "id")
        .appendField("text:")
        .appendField(new Blockly.FieldTextInput("text"), "text")
        .appendField("font:")
        .appendField(new Blockly.FieldDropdown([["Colaborate","Colaborate"], ["Helvetica","Helvetica"], ["OpenSans","OpenSans"]]), "font")
        .appendField(new Blockly.FieldNumber(0), "fontsize")
        .appendField(new Blockly.FieldColour("#000000"), "fontcolor")
        .appendField("icon:")
        .appendField(new Blockly.FieldDropdown([["option","OPTIONNAME"], ["option","OPTIONNAME"], ["option","OPTIONNAME"]]), "icon")
        .appendField(new Blockly.FieldColour("#000000"), "iconcolor")
        .appendField(new Blockly.FieldDropdown([["stack up","STACKUP"],["normal up","NORMALUP"], ["stack","STACK"], ["normal","NORMAL"]]), "type")
        .appendField("back color:")
        .appendField(new Blockly.FieldColour("#99ff99"), "backcolor")
        .appendField("x:")
        .appendField(new Blockly.FieldNumber(0), "x")
        .appendField("y:")
        .appendField(new Blockly.FieldNumber(0), "y")
        .appendField("w:")
        .appendField(new Blockly.FieldNumber(0), "w")
        .appendField("h:")
        .appendField(new Blockly.FieldNumber(0), "h");
    this.appendValueInput("callback")
        .setCheck("gui_callback")
        .appendField("call:");
    this.setInputsInline(true);
    this.setOutput(true, "gui_object");
    this.setColour(120);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_newsimpledropdown'] = function(block) {
  var text_id = block.getFieldValue('id');
  var text_text = block.getFieldValue('text');
  var dropdown_font = block.getFieldValue('font');
  var number_fontsize = block.getFieldValue('fontsize');
  var colour_fontcolor = block.getFieldValue('fontcolor');
  var dropdown_icon = block.getFieldValue('icon');
  var colour_iconcolor = block.getFieldValue('iconcolor');
  var dropdown_type = block.getFieldValue('type');
  var colour_backcolor = block.getFieldValue('backcolor');
  var number_x = block.getFieldValue('x');
  var number_y = block.getFieldValue('y');
  var number_w = block.getFieldValue('w');
  var number_h = block.getFieldValue('h');
  var value_callback = Blockly.Lua.valueToCode(block, 'callback', Blockly.Lua.ORDER_ATOMIC);
  
  var type="proteo.gui.DropDownType.Normal";
  if (dropdown_type=="STACK")
  {
    type="proteo.gui.DropDownType.Stack";
  }
  else if(dropdown_type=="NORMALUP")
  {
    type="proteo.gui.DropDownType.NormalUp";
  }
  else if(dropdown_type=="STACKUP")
  {
    type="proteo.gui.DropDownType.StackUp";
  }

  var callback='""';
  if (value_callback!="")
    callback=value_callback;

  var code = 'proteo.gui.newDropDown("'+text_id+'","'+text_text+'","'+dropdown_font+'",'+number_fontsize+',"'+colour_fontcolor+'","'+dropdown_icon+'","'+colour_iconcolor+'",'+type+',"'+colour_backcolor+'",'+number_x+','+number_y+','+number_w+','+number_h+','+callback+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['gui_newdropdownitem'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create new dropdown item with id:")
        .appendField(new Blockly.FieldTextInput("id"), "id")
        .appendField("text:")
        .appendField(new Blockly.FieldTextInput("text"), "text")
        .appendField("font:")
        .appendField(new Blockly.FieldDropdown([["Colaborate","Colaborate"], ["Helvetica","Helvetica"], ["OpenSans","OpenSans"]]), "font")
        .appendField(new Blockly.FieldNumber(0), "fontsize")
        .appendField(new Blockly.FieldColour("#ff0000"), "fontcolor")
        .appendField("back color:")
        .appendField(new Blockly.FieldColour("#ff0000"), "backcolor")
        .appendField("icon:")
        .appendField(new Blockly.FieldDropdown([["option","OPTIONNAME"], ["option","OPTIONNAME"], ["option","OPTIONNAME"]]), "icon")
        .appendField(new Blockly.FieldColour("#ff0000"), "iconcolor")
        .appendField("w:")
        .appendField(new Blockly.FieldNumber(0), "w")
        .appendField("h:")
        .appendField(new Blockly.FieldNumber(0), "h");
    this.setOutput(true, "gui_object");
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_newdropdownitem'] = function(block) {
  var text_id = block.getFieldValue('id');
  var text_text = block.getFieldValue('text');
  var dropdown_font = block.getFieldValue('font');
  var number_fontsize = block.getFieldValue('fontsize');
  var colour_fontcolor = block.getFieldValue('fontcolor');
  var colour_backcolor = block.getFieldValue('backcolor');
  var dropdown_icon = block.getFieldValue('icon');
  var colour_iconcolor = block.getFieldValue('iconcolor');
  var number_w = block.getFieldValue('w');
  var number_h = block.getFieldValue('h');
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.gui.newDropDownItem("'+text_id+'","'+text_text+'","'+dropdown_font+'",'+number_fontsize+',"'+colour_fontcolor+'","'+colour_backcolor+'","'+dropdown_icon+'","'+colour_iconcolor+'",'+number_w+','+number_h+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['gui_additem'] = {
  init: function() {
    this.appendValueInput("add")
        .setCheck("gui_object")
        .appendField("add:");
    this.appendValueInput("to")
        .setCheck("gui_object")
        .appendField("to:");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_additem'] = function(block) {
  var value_add = Blockly.Lua.valueToCode(block, 'add', Blockly.Lua.ORDER_ATOMIC);
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.gui.addItem('+value_to+','+value_add+')\n';
  return code;
};

//===============================================================================
//===============================================================================

  // Construct the toolbox XML, replacing translated variable names.
  var toolboxText = document.getElementById('toolbox').outerHTML;
  toolboxText = toolboxText.replace(/(^|[^%]){(\w+)}/g,
      function(m, p1, p2) {return p1 + MSG[p2];});
  var toolboxXml = Blockly.Xml.textToDom(toolboxText);

  Code.workspace = Blockly.inject('content_blocks',
      {grid:
          {spacing: 25,
           length: 3,
           colour: '#ccc',
           snap: true},
       media: './blockly/media/',
       rtl: rtl,
       toolbox: toolboxXml,
       zoom:
           {controls: true,
            wheel: true}
      });

  // Add to reserved word list: Local variables in execution environment (runJS)
  // and the infinite loop detection function.
  //Blockly.JavaScript.addReservedWords('code,timeouts,checkTimeout');

  Code.loadBlocks('');

  if ('BlocklyStorage' in window) {
    // Hook a save function onto unload.
    BlocklyStorage.backupOnUnload(Code.workspace);
  }

  Code.tabClick(Code.selected);

  Code.bindClick('trashButton',
      function() {Code.discard(); Code.renderContent();});
  
  //Code.bindClick('runButton', Code.runJS);
  // Disable the link button if page isn't backed by App Engine storage.
  /*var linkButton = document.getElementById('linkButton');
  if ('BlocklyStorage' in window) {
    BlocklyStorage['HTTPREQUEST_ERROR'] = MSG['httpRequestError'];
    BlocklyStorage['LINK_ALERT'] = MSG['linkAlert'];
    BlocklyStorage['HASH_ERROR'] = MSG['hashError'];
    BlocklyStorage['XML_ERROR'] = MSG['xmlError'];
    Code.bindClick(linkButton,
        function() {BlocklyStorage.link(Code.workspace);});
  } else if (linkButton) {
    linkButton.className = 'disabled';
  }*/

Code.bindClick('newButton',
        function() {

          var new_file = prompt("Please enter "+typeMenu.options[typeMenu.selectedIndex].value+" name", "New_"+typeMenu.options[typeMenu.selectedIndex].value);

          //Check if is valid

          fileMenu.options.add(new Option(new_file, new_file));
  });        
  Code.bindClick('saveButton',
        function() {

          var http = new XMLHttpRequest();
          var url = ".";
          var file=typeMenu.options[typeMenu.selectedIndex].value+"/"+fileMenu.options[fileMenu.selectedIndex].value;
          var path="/edit/"+file;//plugin/prova";
          var lua_script=Blockly.Lua.workspaceToCode(Code.workspace);
          var xmlDom = Blockly.Xml.workspaceToDom(Code.workspace);
          var xml_script = Blockly.Xml.domToPrettyText(xmlDom);

          var jsonData=JSON.stringify({ lua: lua_script,xml:xml_script});
          var appKey="}~?d1BE+\"d5?TZ(j`{+n`pfK&*2U(WPy";
          
          var hash = CryptoJS.HmacSHA256(path+jsonData, appKey);
            var hashInBase64 = CryptoJS.enc.Base64.stringify(hash);
  
          http.onreadystatechange = function() {
            if(http.readyState == 4 && http.status == 200) {
              var data = JSON.parse(http.responseText);
              }              
          }
          //console.log("Data: "+jsonData)
          http.open("PUT", url+path, true);
          http.setRequestHeader("X-HMAC", hashInBase64);
          http.setRequestHeader("X-TOKEN", token);
          http.send(jsonData);

          //BlocklyStorage.link(Code.workspace);

        });

  Code.bindClick('loadButton',
        function() {

          var http = new XMLHttpRequest();
          var url = ".";
          var file=typeMenu.options[typeMenu.selectedIndex].value+"/"+fileMenu.options[fileMenu.selectedIndex].value;
          //console.log(file);
          var path="/edit/"+file;//plugin/prova";

          var appKey="}~?d1BE+\"d5?TZ(j`{+n`pfK&*2U(WPy";
          
          var hash = CryptoJS.HmacSHA256(path+token, appKey);
          var hashInBase64 = CryptoJS.enc.Base64.stringify(hash);
  
          http.onreadystatechange = function() {
            if(http.readyState == 4 && http.status == 200) {
              var data = JSON.parse(http.responseText);

                //console.log("Data: "+data["xml"])
                Code.workspace.clear();
                var xml = Blockly.Xml.textToDom(data["xml"]);
                Blockly.Xml.domToWorkspace(xml, Code.workspace);
              }              
          }
          //console.log("Data: "+jsonData)
          http.open("GET", url+path, true);
          http.setRequestHeader("X-HMAC", hashInBase64);
          http.setRequestHeader("X-TOKEN", token);
          http.send();

          //BlocklyStorage.link(Code.workspace);

        });

  for (var i = 0; i < Code.TABS_.length; i++) {
    var name = Code.TABS_[i];
    Code.bindClick('tab_' + name,
        function(name_) {return function() {Code.tabClick(name_);};}(name));
  }
  Code.bindClick('tab_code', function(e) {
    if (e.target !== document.getElementById('tab_code')) {
      // Prevent clicks on child codeMenu from triggering a tab click.
      return;
    }
    Code.changeCodingLanguage();
  });

  onresize();
  Blockly.svgResize(Code.workspace);

  // Lazy-load the syntax-highlighting.
  window.setTimeout(Code.importPrettify, 1);
};

/**
 * Initialize the page language.
 */
Code.initLanguage = function() {
  // Set the HTML's language and direction.
  var rtl = Code.isRtl();
  document.dir = rtl ? 'rtl' : 'ltr';
  document.head.parentElement.setAttribute('lang', Code.LANG);

  // Sort languages alphabetically.
  var languages = [];
  for (var lang in Code.LANGUAGE_NAME) {
    languages.push([Code.LANGUAGE_NAME[lang], lang]);
  }
  var comp = function(a, b) {
    // Sort based on first argument ('English', 'Русский', '简体字', etc).
    if (a[0] > b[0]) return 1;
    if (a[0] < b[0]) return -1;
    return 0;
  };
  languages.sort(comp);
  // Populate the language selection menu.
  var languageMenu = document.getElementById('languageMenu');
  languageMenu.options.length = 0;
  for (var i = 0; i < languages.length; i++) {
    var tuple = languages[i];
    var lang = tuple[tuple.length - 1];
    var option = new Option(tuple[0], lang);
    if (lang == Code.LANG) {
      option.selected = true;
    }
    languageMenu.options.add(option);
  }
  languageMenu.addEventListener('change', Code.changeLanguage, true);

//===============================================================================
//  FILE
//===============================================================================

  var fileMenu = document.getElementById('fileMenu');
  var typeMenu = document.getElementById('typeMenu');
  typeMenu.options.add(new Option("", 0));
  typeMenu.options.add(new Option("Script", "script"));
  typeMenu.options.add(new Option("Plugin", "plugin"));
  typeMenu.addEventListener('change', Code.changeType, true);

  typeMenu.addEventListener('change', function() {  
    
    //typeMenu.options[typeMenu.selectedIndex].value,

    var http = new XMLHttpRequest();
    var url = ".";
    var path="/edit/plugins";

    if (typeMenu.selectedIndex==1)
    {
      console.log("Scripts");
      path="/edit/scripts";
    }
    fileMenu.options.length = 0

    var appKey="}~?d1BE+\"d5?TZ(j`{+n`pfK&*2U(WPy";
          
    var hash = CryptoJS.HmacSHA256(path+token, appKey);
    var hashInBase64 = CryptoJS.enc.Base64.stringify(hash);
  
    http.onreadystatechange = function() {
      if(http.readyState == 4 && http.status == 200) {
              
          var data = JSON.parse(http.responseText);
          //fileMenu.options.length = 0
          fileMenu.options.add(new Option("", 0));
          for (var i = 0; i < data["list"].length; i++) {
            fileMenu.options.add(new Option(data["list"][i], data["list"][i]));
          }
          
          //console.log("Data: "+data["plugin"][1])

      }              
    }
          
    //console.log("Data: "+jsonData)
    http.open("GET", url+path, true);
    http.setRequestHeader("X-HMAC", hashInBase64);
    http.setRequestHeader("X-TOKEN", token);
    http.send();
  });

  // Populate the coding language selection menu.
  var codeMenu = document.getElementById('code_menu');
  codeMenu.options.length = 0;
  for (var i = 1; i < Code.TABS_.length; i++) {
    codeMenu.options.add(new Option(Code.TABS_DISPLAY_[i], Code.TABS_[i]));
  }
  codeMenu.addEventListener('change', Code.changeCodingLanguage);

  // Inject language strings.
  document.title += ' ' + MSG['title'];
  document.getElementById('title').textContent = MSG['title'];
  document.getElementById('tab_blocks').textContent = MSG['blocks'];

  //document.getElementById('linkButton').title = MSG['linkTooltip'];
  //document.getElementById('runButton').title = MSG['runTooltip'];
  document.getElementById('trashButton').title = MSG['trashTooltip'];
};

/**
 * Execute the user's code.
 * Just a quick and dirty eval.  Catch infinite loops.
 */
/*Code.runJS = function() {
  Blockly.JavaScript.INFINITE_LOOP_TRAP = 'checkTimeout();\n';
  var timeouts = 0;
  var checkTimeout = function() {
    if (timeouts++ > 1000000) {
      throw MSG['timeout'];
    }
  };
  var code = Blockly.JavaScript.workspaceToCode(Code.workspace);
  Blockly.JavaScript.INFINITE_LOOP_TRAP = null;
  try {
    eval(code);
  } catch (e) {
    alert(MSG['badCode'].replace('%1', e));
  }
};
*/
/**
 * Discard all blocks from the workspace.
 */
Code.discard = function() {
  var count = Code.workspace.getAllBlocks(false).length;
  if (count < 2 ||
      window.confirm(Blockly.Msg['DELETE_ALL_BLOCKS'].replace('%1', count))) {
    Code.workspace.clear();
    if (window.location.hash) {
      window.location.hash = '';
    }
  }
};

// Load the Code demo's language strings.
document.write('<script src="blockly/msg/' + Code.LANG + '.js"></script>\n');
// Load Blockly's language strings.
document.write('<script src="./blockly/msg/js/' + Code.LANG + '.js"></script>\n');

window.addEventListener('load', Code.init);
