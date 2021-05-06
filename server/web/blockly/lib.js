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
        .appendField("show form")

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

Blockly.Blocks['pose_detect'] = {
  init: function() {
    this.appendValueInput("img")
        .setCheck("opencv_img")
        .appendField("detect pose with")
        .appendField(new Blockly.FieldDropdown([["blazepose","tfl_blazepose"], ["humanpose","opencv_humanpose"]]), "mode")
        .appendField("from image");
    this.setOutput(true, null);
    this.setColour(330);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['pose_detect'] = function(block) {
  var dropdown_mode = block.getFieldValue('mode');
  var value_img = Blockly.Lua.valueToCode(block, 'img', Blockly.Lua.ORDER_ATOMIC);

  var code=""
  if(dropdown_mode=="tfl_blazepose")
  {
    var bbox = Blockly.Lua.provideFunction_(
      '',
      [ 'require "tfl_blazepose"']);

    code = "invoke_pose_detect("+value_img+")";
  }
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['pose_generate_landmark_input_image'] = {
  init: function() {
    this.appendValueInput("img")
        .setCheck(null)
        .appendField("generate blazepose landmark input from image");
    this.appendValueInput("bbox")
        .setCheck(null)
        .appendField("and bbox");
    this.setInputsInline(true);
    this.setOutput(true, "opencv_img");
    this.setColour(330);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['pose_generate_landmark_input_image'] = function(block) {
  var value_img = Blockly.Lua.valueToCode(block, 'img', Blockly.Lua.ORDER_ATOMIC);
  var value_bbox = Blockly.Lua.valueToCode(block, 'bbox', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'generate_pose_landmark_input_image('+value_img+','+value_bbox+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['pose_landamark'] = {
  init: function() {
    this.appendValueInput("img")
        .setCheck("opencv_img")
        .appendField("detect landmark with")
        .appendField(new Blockly.FieldDropdown([["blazepose","tfl_blazepose"], ["humanpose","opencv_humanpose"]]), "mode")
        .appendField("from image");
    this.setOutput(true, null);
    this.setColour(330);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['pose_landamark'] = function(block) {
  var dropdown_mode = block.getFieldValue('mode');
  var value_img = Blockly.Lua.valueToCode(block, 'img', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'invoke_pose_landmark('+value_img+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};


Blockly.Blocks['face_detect'] = {
  init: function() {
    this.appendValueInput("img")
        .setCheck("opencv_img")
        .appendField("detect face with")
        .appendField(new Blockly.FieldDropdown([["blazeface","tfl_blazeface"], ["openface","opencv_openface"]]), "mode")
        .appendField("from image");
    this.setOutput(true, null);
    this.setColour(330);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['face_detect'] = function(block) {
  var dropdown_mode = block.getFieldValue('mode');
  var value_img = Blockly.Lua.valueToCode(block, 'img', Blockly.Lua.ORDER_ATOMIC);

  var code=""
  if(dropdown_mode=="tfl_blazeface")
  {
    var bbox = Blockly.Lua.provideFunction_(
      '',
      [ 'require "tfl_blazeface"']);

    code = "invoke_face_detect("+value_img+")";
  }
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['face_generate_landmark_input_image'] = {
  init: function() {
    this.appendValueInput("img")
        .setCheck(null)
        .appendField("generate blazeface landmark input from image");
    this.appendValueInput("bbox")
        .setCheck(null)
        .appendField("and bbox");
    this.setInputsInline(true);
    this.setOutput(true, "opencv_img");
    this.setColour(330);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['face_generate_landmark_input_image'] = function(block) {
  var value_img = Blockly.Lua.valueToCode(block, 'img', Blockly.Lua.ORDER_ATOMIC);
  var value_bbox = Blockly.Lua.valueToCode(block, 'bbox', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'generate_face_landmark_input_image('+value_img+','+value_bbox+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['face_landamark'] = {
  init: function() {
    this.appendValueInput("img")
        .setCheck("opencv_img")
        .appendField("detect facemesh with")
        .appendField(new Blockly.FieldDropdown([["blazeface","tfl_blazeface"], ["openface","opencv_openface"]]), "mode")
        .appendField("from image");
    this.setOutput(true, null);
    this.setColour(330);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['face_landamark'] = function(block) {
  var dropdown_mode = block.getFieldValue('mode');
  var value_img = Blockly.Lua.valueToCode(block, 'img', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'invoke_face_landmark('+value_img+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['pose_showlandmark'] = {
  init: function() {
    this.appendValueInput("bbox")
        .setCheck(null)
        .appendField("draw bbox");
    this.appendValueInput("landmark")
        .setCheck(null)
        .appendField("and landmarks");
    this.appendValueInput("img")
        .setCheck("opencv_img")
        .appendField("to image");
    this.setInputsInline(true);
    this.setOutput(true, null);
    this.setColour(330);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['pose_showlandmark'] = function(block) {
  var value_bbox = Blockly.Lua.valueToCode(block, 'bbox', Blockly.Lua.ORDER_ATOMIC);
  var value_landmark = Blockly.Lua.valueToCode(block, 'landmark', Blockly.Lua.ORDER_ATOMIC);
  var value_img = Blockly.Lua.valueToCode(block, 'img', Blockly.Lua.ORDER_ATOMIC);
  
  var tfl_utility = Blockly.Lua.provideFunction_(
      'req_tfl_utility',
      [ 'require "tfl_utility"']);

    code = "show_landmark("+value_bbox+","+value_img+","+value_landmark+")";


  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['pose_showpose'] = {
  init: function() {
    this.appendValueInput("pose")
        .setCheck(null)
        .appendField("draw pose");
    this.appendValueInput("img")
        .setCheck("opencv_img")
        .appendField("to image");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(330);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['pose_showpose'] = function(block) {
  var value_pose = Blockly.Lua.valueToCode(block, 'pose', Blockly.Lua.ORDER_ATOMIC);
  var value_img = Blockly.Lua.valueToCode(block, 'img', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'show_pose('+value_pose+','+value_img+')\n';
  return code;
};

Blockly.Blocks['face_showfacemesh'] = {
  init: function() {
    this.appendValueInput("pose")
        .setCheck(null)
        .appendField("draw facemesh");
    this.appendValueInput("img")
        .setCheck("opencv_img")
        .appendField("to image");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(330);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['face_showfacemesh'] = function(block) {
  var value_pose = Blockly.Lua.valueToCode(block, 'pose', Blockly.Lua.ORDER_ATOMIC);
  var value_img = Blockly.Lua.valueToCode(block, 'img', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'show_facemesh('+value_pose+','+value_img+')\n';
  return code;
};

Blockly.Blocks['shape_svg2shape'] = {
  init: function() {
    this.appendValueInput("svg")
        .setCheck(null)
        .appendField("insert svg shape");
    this.appendValueInput("shape")
        .setCheck(null)
        .appendField("into");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(330);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['shape_svg2shape'] = function(block) {
  var value_svg = Blockly.Lua.valueToCode(block, 'svg', Blockly.Lua.ORDER_ATOMIC);
  var value_shape = Blockly.Lua.valueToCode(block, 'shape', Blockly.Lua.ORDER_ATOMIC);
 
  var skl_utility = Blockly.Lua.provideFunction_(
      'req_skl_utility',
      [ 'require "skl_utility"']);

    code = "svg2shape("+value_svg+","+value_shape+")\n";


  return code;
};

Blockly.Blocks['skl_json2skl'] = {
  init: function() {
    this.appendValueInput("json")
        .setCheck(null)
        .appendField("insert json skeleton");
    this.appendValueInput("skl")
        .setCheck(null)
        .appendField("into");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(330);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['skl_json2skl'] = function(block) {
  var value_json = Blockly.Lua.valueToCode(block, 'json', Blockly.Lua.ORDER_ATOMIC);
  var value_skl = Blockly.Lua.valueToCode(block, 'skl', Blockly.Lua.ORDER_ATOMIC);
 
  var skl_utility = Blockly.Lua.provideFunction_(
      'req_skl_utility',
      [ 'require "skl_utility"']);

    code = "json2skl("+value_json+","+value_skl+")\n";


  return code;
};

Blockly.Blocks['inspect'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("print inspect")
        .appendField(new Blockly.FieldVariable("item"), "data");
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(330);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['inspect'] = function(block) {
  var variable_data = Blockly.Lua.variableDB_.getName(block.getFieldValue('data'), Blockly.Variables.NAME_TYPE);
  
  var inspect = Blockly.Lua.provideFunction_(
    'inspect',
    [ 'local ' + Blockly.Lua.FUNCTION_NAME_PLACEHOLDER_ + '=require "inspect"']);

  var code = "print("+inspect+"("+variable_data+"))\n";

  return code;
};

Blockly.Blocks['create_2d_array'] = {
  init: function() {
    this.appendValueInput("data")
        .setCheck(null)
        .appendField("create array of size")
        .appendField(new Blockly.FieldNumber(2, 1), "rows")
        .appendField(new Blockly.FieldNumber(2, 1), "cols")
        .appendField("and populate with");
    this.setOutput(true, "2d_array");
    this.setColour(260);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['create_2d_array'] = function(block) {
  var number_rows = block.getFieldValue('rows');
  var number_cols = block.getFieldValue('cols');
  var value_data = Blockly.Lua.valueToCode(block, 'data', Blockly.Lua.ORDER_ATOMIC);
  
  var code = '{';
  for (var i = 0; i < number_rows; i++)
  {
    code = code + '{'+value_data;
    for (var j = 1; j < number_cols; j++)
    {
      code = code + ',' + value_data;
    }
    code = code + '},\n';
  }
  code = code + '}';

  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['get_2d_array_element'] = {
  init: function() {
    this.appendValueInput("row")
        .setCheck(null)
        .appendField("get element");
    this.appendValueInput("col")
        .setCheck(null);
    this.appendValueInput("from")
        .setCheck("2d_array")
        .appendField("from");
    this.setInputsInline(true);
    this.setOutput(true, null);
    this.setColour(260);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['get_2d_array_element'] = function(block) {
  var value_row = Blockly.Lua.valueToCode(block, 'row', Blockly.Lua.ORDER_ATOMIC);
  var value_col = Blockly.Lua.valueToCode(block, 'col', Blockly.Lua.ORDER_ATOMIC);
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = value_from+'['+value_row+']['+value_col+']';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['set_2d_array_element'] = {
  init: function() {
    this.appendValueInput("row")
        .setCheck(null)
        .appendField("set element");
    this.appendValueInput("col")
        .setCheck(null);
    this.appendValueInput("array")
        .setCheck("2d_array")
        .appendField("of");
    this.appendValueInput("value")
        .setCheck(null)
        .appendField("to");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(260);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['set_2d_array_element'] = function(block) {
  var value_row = Blockly.Lua.valueToCode(block, 'row', Blockly.Lua.ORDER_ATOMIC);
  var value_col = Blockly.Lua.valueToCode(block, 'col', Blockly.Lua.ORDER_ATOMIC);
  var value_array = Blockly.Lua.valueToCode(block, 'array', Blockly.Lua.ORDER_ATOMIC);
  var value_value = Blockly.Lua.valueToCode(block, 'value', Blockly.Lua.ORDER_ATOMIC);
  var code = value_array+'['+value_row+']['+value_col+']='+value_value+'\n';
  return code;
};

Blockly.Blocks['make_2d_array_from_text'] = {
  init: function() {
    this.appendValueInput("data")
        .setCheck("String")
        .appendField("make array of size")
        .appendField(new Blockly.FieldNumber(2, 1), "rows")
        .appendField(new Blockly.FieldNumber(2, 1), "cols")
        .appendField("from text");
    this.appendValueInput("delim")
        .setCheck("String")
        .appendField("with delimiter");
    this.setInputsInline(true);
    this.setOutput(true, "2d_array");
    this.setColour(260);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['make_2d_array_from_text'] = function(block) {
  var number_rows = block.getFieldValue('rows');
  var number_cols = block.getFieldValue('cols');
  var value_data = Blockly.Lua.valueToCode(block, 'data', Blockly.Lua.ORDER_ATOMIC);
  var value_delim = Blockly.Lua.valueToCode(block, 'delim', Blockly.Lua.ORDER_ATOMIC);

  var data = value_data.replace(/['"]+/g, '').split(value_delim.replace(/['"]+/g, ''));

  var code = '{';
  for (var i = 0; i < number_rows; i++)
  {
    code = code + '{'+data[i*number_cols];
    for (var j = 1; j < number_cols; j++)
    {
      code = code + ',' + data[j+i*number_cols];
    }
    code = code + '},\n';
  }
  code = code + '}';

  return [code, Blockly.Lua.ORDER_ATOMIC];
};
