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
var appKey="}~?d1BE+\"d5?TZ(j`{+n`pfK&*2U(WPy";

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
        .appendField("get key:");
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

Blockly.Blocks['close_function'] = {
  init: function() {
    this.appendStatementInput("close")
        .setCheck(null)
        .appendField("close");
    this.setColour(290);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['close_function'] = function(block) {
  var statements_close = Blockly.Lua.statementToCode(block, 'close');
  var code = 'function close() \n'+statements_close+' end\n';
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
        .appendField(new Blockly.FieldDropdown([["Request","ZMQ_REQ"], ["Reply","ZMQ_REP"], ["Pair","ZMQ_PAIR"]]), "type");
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

Blockly.Blocks['zmq_closesocket'] = {
  init: function() {
    this.appendValueInput("context")
        .setCheck("zmq_socket")
        .appendField("close socket");
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(285);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['zmq_closesocket'] = function(block) {
  var value_context = Blockly.Lua.valueToCode(block, 'context', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.zmq.socket_close('+value_context+')\n';
  return code;
};

Blockly.Blocks['zmq_bindconnect'] = {
  init: function() {
    this.appendValueInput("socket")
        .setCheck("zmq_socket")
        .appendField(new Blockly.FieldDropdown([["bind","bind"], ["connect","connect"], ["unbind","unbind"], ["disconnect","disconnect"]]), "operation")
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

    var http = new XMLHttpRequest();
    var url = ".";
    var path="/edit/items";
  
    var hash = CryptoJS.HmacSHA256(path+token, appKey);
    var hashInBase64 = CryptoJS.enc.Base64.stringify(hash);
  
    http.onreadystatechange = function() {
      if(http.readyState == 4 && http.status == 200) {
              
          var data = JSON.parse(http.responseText);

          for (var j = 0; j < data["items"].length; j++) {
            var category =  Code.workspace.getToolbox().getToolboxItemById(data["items"][j]["id"]);
            if (data["items"][j]["hide"])
            { 
              if(category==null)
              {
                console.log("Category: "+data["items"][j]["id"])
              }
              else
              {
                category.hide();
              }
            }
          }
      }              
    }
          
    //console.log("Data: "+jsonData)
    http.open("GET", url+path, true);
    http.setRequestHeader("X-HMAC", hashInBase64);
    http.setRequestHeader("X-TOKEN", token);
    http.send();




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
