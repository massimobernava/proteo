//===============================================================================
//  DEMO
//===============================================================================

Blockly.Blocks['sequences_createpage'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create sequences page with:");
    this.appendValueInput("main")
        .setCheck("String")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("main image");
    this.appendValueInput("option1")
        .setCheck("String")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("correct")
        .appendField(new Blockly.FieldCheckbox("FALSE"), "correct1")
        .appendField("option 1 image");
    this.appendValueInput("option2")
        .setCheck("String")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("correct")
        .appendField(new Blockly.FieldCheckbox("FALSE"), "correct2")
        .appendField("option 2 image");
    this.appendValueInput("option3")
        .setCheck("String")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("correct")
        .appendField(new Blockly.FieldCheckbox("FALSE"), "correct3")
        .appendField("option 3 image");
    this.appendValueInput("option4")
        .setCheck("String")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("correct")
        .appendField(new Blockly.FieldCheckbox("FALSE"), "correct4")
        .appendField("option 4 image");
    this.appendValueInput("option5")
        .setCheck("String")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("correct")
        .appendField(new Blockly.FieldCheckbox("FALSE"), "correct5")
        .appendField("option 5 image");
    this.appendValueInput("option6")
        .setCheck("String")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("correct")
        .appendField(new Blockly.FieldCheckbox("FALSE"), "correct6")
        .appendField("option 6 image");
    this.appendValueInput("option7")
        .setCheck("String")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("correct")
        .appendField(new Blockly.FieldCheckbox("FALSE"), "correct7")
        .appendField("option 7 image");
    this.appendValueInput("option8")
        .setCheck("String")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("correct")
        .appendField(new Blockly.FieldCheckbox("FALSE"), "correct8")
        .appendField("option 8 image");
    this.appendDummyInput()
        .appendField("shuffle answers")
        .appendField(new Blockly.FieldCheckbox("TRUE"), "shuffle");
    this.appendDummyInput()
        .appendField("background color")
        .appendField(new Blockly.FieldColour("#ffffff"), "color");
    this.appendStatementInput("correct")
        .setCheck(null)
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("if the answer is correct do");
    this.appendStatementInput("wrong")
        .setCheck(null)
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("otherwise if the answer is wrong do");
    this.setInputsInline(false);
    this.setOutput(true, "generic_page");
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['sequences_createpage'] = function(block) {
  var value_main = Blockly.Lua.valueToCode(block, 'main', Blockly.Lua.ORDER_ATOMIC);
  var checkbox_correct1 = block.getFieldValue('correct1') == 'TRUE';
  var value_option1 = Blockly.Lua.valueToCode(block, 'option1', Blockly.Lua.ORDER_ATOMIC);
  var checkbox_correct2 = block.getFieldValue('correct2') == 'TRUE';
  var value_option2 = Blockly.Lua.valueToCode(block, 'option2', Blockly.Lua.ORDER_ATOMIC);
  var checkbox_correct3 = block.getFieldValue('correct3') == 'TRUE';
  var value_option3 = Blockly.Lua.valueToCode(block, 'option3', Blockly.Lua.ORDER_ATOMIC);
  var checkbox_correct4 = block.getFieldValue('correct4') == 'TRUE';
  var value_option4 = Blockly.Lua.valueToCode(block, 'option4', Blockly.Lua.ORDER_ATOMIC);
  var checkbox_correct5 = block.getFieldValue('correct5') == 'TRUE';
  var value_option5 = Blockly.Lua.valueToCode(block, 'option5', Blockly.Lua.ORDER_ATOMIC);
  var checkbox_correct6 = block.getFieldValue('correct6') == 'TRUE';
  var value_option6 = Blockly.Lua.valueToCode(block, 'option6', Blockly.Lua.ORDER_ATOMIC);
  var checkbox_correct7 = block.getFieldValue('correct7') == 'TRUE';
  var value_option7 = Blockly.Lua.valueToCode(block, 'option7', Blockly.Lua.ORDER_ATOMIC);
  var checkbox_correct8 = block.getFieldValue('correct8') == 'TRUE';
  var value_option8 = Blockly.Lua.valueToCode(block, 'option8', Blockly.Lua.ORDER_ATOMIC);
  var checkbox_shuffle = block.getFieldValue('shuffle') == 'TRUE';
  var colour_color = block.getFieldValue('color');
  var statements_correct = Blockly.Lua.statementToCode(block, 'correct');
  var statements_wrong = Blockly.Lua.statementToCode(block, 'wrong');
  

  var demo_lib = Blockly.Lua.provideFunction_(
    'demo_lib',
    [ 'local ' + Blockly.Lua.FUNCTION_NAME_PLACEHOLDER_ + '=require "demo_lib"\n function touch(x,y) '+Blockly.Lua.FUNCTION_NAME_PLACEHOLDER_ + '.touch(x,y) end']);

  var code = demo_lib+".create_sequence_page({main_image="+value_main+
  ",option1_image="+value_option1+
  ",option2_image="+value_option2+
  ",option3_image="+value_option3+
  ",option4_image="+value_option4+
  ",option5_image="+value_option5+
  ",option6_image="+value_option6+
  ",option7_image="+value_option7+
  ",option8_image="+value_option8+
  ",option1_correct="+checkbox_correct1+
  ",option2_correct="+checkbox_correct2+
  ",option3_correct="+checkbox_correct3+
  ",option4_correct="+checkbox_correct4+
  ",option5_correct="+checkbox_correct5+
  ",option6_correct="+checkbox_correct6+
  ",option7_correct="+checkbox_correct7+
  ",option8_correct="+checkbox_correct8+
  ",function_correct=function() "+statements_correct+" end"+
  ",function_wrong=function() "+statements_wrong+" end"+
  ",shuffle="+checkbox_shuffle+
  ",color='"+colour_color+"'})\n";

  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['sequences_showpage'] = {
  init: function() {
    this.appendValueInput("page")
        .setCheck("generic_page")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("show page");
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['sequences_showpage'] = function(block) {
  var value_page = Blockly.Lua.valueToCode(block, 'page', Blockly.Lua.ORDER_ATOMIC);
  
  var demo_lib = Blockly.Lua.provideFunction_(
    'demo_lib',
      [ 'local ' + Blockly.Lua.FUNCTION_NAME_PLACEHOLDER_ + '=require "demo_lib"\n function touch(x,y) '+Blockly.Lua.FUNCTION_NAME_PLACEHOLDER_ + '.touch(x,y) end']);

  var code = demo_lib+".show("+value_page+")\n";

  return code;
};

Blockly.Blocks['sequences_createavatar'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create avatar with:");
    this.appendValueInput("default")
        .setCheck("String")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("default animation");
    this.appendValueInput("alone")
        .setCheck("String")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("\"alone\" animation");
    this.appendValueInput("speaking")
        .setCheck("String")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("\"speaking\" animation");
    this.appendValueInput("wow")
        .setCheck("String")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("\"wow\" animation");
    this.setInputsInline(false);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['sequences_createavatar'] = function(block) {
  var value_default = Blockly.Lua.valueToCode(block, 'default', Blockly.Lua.ORDER_ATOMIC);
  var value_alone = Blockly.Lua.valueToCode(block, 'alone', Blockly.Lua.ORDER_ATOMIC);
  var value_speaking = Blockly.Lua.valueToCode(block, 'speaking', Blockly.Lua.ORDER_ATOMIC);
  var value_wow = Blockly.Lua.valueToCode(block, 'wow', Blockly.Lua.ORDER_ATOMIC);
  
    var demo_lib = Blockly.Lua.provideFunction_(
    'demo_lib',
      [ 'local ' + Blockly.Lua.FUNCTION_NAME_PLACEHOLDER_ + '=require "demo_lib"\n function touch(x,y) '+Blockly.Lua.FUNCTION_NAME_PLACEHOLDER_ + '.touch(x,y) end']);

  var code = demo_lib+".create_avatar("+value_default+","+value_alone+","+value_speaking+","+value_wow+")\n";

  return code;
};

Blockly.Blocks['sequences_loadsound'] = {
  init: function() {
    this.appendValueInput("load")
        .setCheck("String")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("load sound");
    this.setInputsInline(false);
    this.setOutput(true, null);
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['sequences_loadsound'] = function(block) {
  var value_load = Blockly.Lua.valueToCode(block, 'load', Blockly.Lua.ORDER_ATOMIC);
  
  var demo_lib = Blockly.Lua.provideFunction_(
    'demo_lib',
      [ 'local ' + Blockly.Lua.FUNCTION_NAME_PLACEHOLDER_ + '=require "demo_lib"\n function touch(x,y) '+Blockly.Lua.FUNCTION_NAME_PLACEHOLDER_ + '.touch(x,y) end']);

  var code = demo_lib+".load_sound("+value_load+")\n";

  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['sequences_playsound'] = {
  init: function() {
    this.appendValueInput("sound")
        .setCheck(null)
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("play sound");
    this.appendDummyInput()
        .appendField("with avatar")
        .appendField(new Blockly.FieldDropdown([["default","default"], ["alone","alone"], ["speaking","speaking"], ["wow","wow"]]), "animation")
        .appendField("animation");
    this.setInputsInline(false);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};


Blockly.Lua['sequences_playsound'] = function(block) {
  var value_sound = Blockly.Lua.valueToCode(block, 'sound', Blockly.Lua.ORDER_ATOMIC);
  var dropdown_animation = block.getFieldValue('animation');
  
  var demo_lib = Blockly.Lua.provideFunction_(
    'demo_lib',
      [ 'local ' + Blockly.Lua.FUNCTION_NAME_PLACEHOLDER_ + '=require "demo_lib"\n function touch(x,y) '+Blockly.Lua.FUNCTION_NAME_PLACEHOLDER_ + '.touch(x,y) end']);


  var code = demo_lib+".play_sound("+value_sound+",'"+dropdown_animation+"')\n";

  return code;
};

Blockly.Blocks['sequences_facedetect'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("If the user");
    this.appendStatementInput("attentive")
        .setCheck(null)
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("pays attention do");
    this.appendStatementInput("distracted")
        .setCheck(null)
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField(" is distracted for more than")
        .appendField(new Blockly.FieldNumber(1, 0), "sec")
        .appendField("s do");
    this.setInputsInline(false);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['sequences_facedetect'] = function(block) {
  var statements_attentive = Blockly.Lua.statementToCode(block, 'attentive');
  var number_sec = block.getFieldValue('sec');
  var statements_distracted = Blockly.Lua.statementToCode(block, 'distracted');
  
  var demo_lib = Blockly.Lua.provideFunction_(
    'demo_lib',
    [ 'local ' + Blockly.Lua.FUNCTION_NAME_PLACEHOLDER_ + '=require "demo_lib"\n function touch(x,y) '+Blockly.Lua.FUNCTION_NAME_PLACEHOLDER_ + '.touch(x,y) end']);

  var code = demo_lib+".face_detect({function_attentive=function() "+statements_attentive+" end"+
  ",function_distracted=function() "+statements_distracted+" end"+
  ",sec="+number_sec+"})\n";

  return code;
};

Blockly.Blocks['sequences_createinfopage'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create info page with:");
    this.appendValueInput("field1")
        .setCheck("generic_field")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("field 1");
    this.appendValueInput("field2")
        .setCheck("generic_field")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("field 2");

    this.appendValueInput("field3")
        .setCheck("generic_field")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("field 3");

    this.appendValueInput("field4")
        .setCheck("generic_field")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("field 4");
    this.appendValueInput("field5")
        .setCheck("generic_field")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("field 5");
    this.appendValueInput("field6")
        .setCheck("generic_field")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("field 6");
    this.appendValueInput("field7")
        .setCheck("generic_field")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("field 7");
    this.appendValueInput("field8")
        .setCheck("generic_field")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("field 8");
    this.appendValueInput("field9")
        .setCheck("generic_field")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("field 9");
    this.appendValueInput("field10")
        .setCheck("generic_field")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("field 10");

    this.appendDummyInput()
        .appendField("background color")
        .appendField(new Blockly.FieldColour("#ffffff"), "color");
    this.appendDummyInput()
        .appendField("button text")
        .appendField(new Blockly.FieldTextInput("next"), "text");
    this.appendStatementInput("action")
        .setCheck(null)
        .appendField("on button click do");
    this.setInputsInline(false);
    this.setOutput(true, "generic_page");
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['sequences_createinfopage'] = function(block) {
  var value_field1 = Blockly.Lua.valueToCode(block, 'field1', Blockly.Lua.ORDER_ATOMIC);
  var value_field2 = Blockly.Lua.valueToCode(block, 'field2', Blockly.Lua.ORDER_ATOMIC);
  var value_field3 = Blockly.Lua.valueToCode(block, 'field3', Blockly.Lua.ORDER_ATOMIC);
  var value_field4 = Blockly.Lua.valueToCode(block, 'field4', Blockly.Lua.ORDER_ATOMIC);
  var value_field5 = Blockly.Lua.valueToCode(block, 'field5', Blockly.Lua.ORDER_ATOMIC);
  var value_field6 = Blockly.Lua.valueToCode(block, 'field6', Blockly.Lua.ORDER_ATOMIC);
  var value_field7 = Blockly.Lua.valueToCode(block, 'field7', Blockly.Lua.ORDER_ATOMIC);
  var value_field8 = Blockly.Lua.valueToCode(block, 'field8', Blockly.Lua.ORDER_ATOMIC);
  var value_field9 = Blockly.Lua.valueToCode(block, 'field9', Blockly.Lua.ORDER_ATOMIC);
  var value_field10 = Blockly.Lua.valueToCode(block, 'field10', Blockly.Lua.ORDER_ATOMIC);

  var text_text = block.getFieldValue('text');
  var statements_action = Blockly.Lua.statementToCode(block, 'action');
  var colour_color = block.getFieldValue('color');
  // TODO: Assemble Lua into code variable.
  var demo_lib = Blockly.Lua.provideFunction_(
    'demo_lib',
    [ 'local ' + Blockly.Lua.FUNCTION_NAME_PLACEHOLDER_ + '=require "demo_lib"\n function touch(x,y) '+Blockly.Lua.FUNCTION_NAME_PLACEHOLDER_ + '.touch(x,y) end']);

  var code = demo_lib+".create_info_page({"+
  (value_field1==""?"":("field1_array="+value_field1))+
  (value_field2==""?"":(",field2_array="+value_field2))+
  (value_field3==""?"":(",field3_array="+value_field3))+
  (value_field4==""?"":(",field4_array="+value_field4))+
  (value_field5==""?"":(",field5_array="+value_field5))+
  (value_field6==""?"":(",field6_array="+value_field6))+
  (value_field7==""?"":(",field7_array="+value_field7))+
  (value_field8==""?"":(",field8_array="+value_field8))+
  (value_field9==""?"":(",field9_array="+value_field9))+
  (value_field10==""?"":(",field10_array="+value_field10))+
  ",function_next=function() "+statements_action+" end"+
  ",button_txt='"+text_text+"'"+
  ",color='"+colour_color+"'})\n";

  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['sequences_txtfield'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create text field with title")
        .appendField(new Blockly.FieldTextInput("default"), "title");
    this.setInputsInline(false);
    this.setOutput(true, "generic_field");
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['sequences_txtfield'] = function(block) {
  var text_title = block.getFieldValue('title');
  // TODO: Assemble Lua into code variable.
  var code = '{field_type="txt",title="'+text_title+'"}';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['sequences_saveall'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("save all data");
    this.setInputsInline(false);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['sequences_saveall'] = function(block) {
  // TODO: Assemble Lua into code variable.
 var demo_lib = Blockly.Lua.provideFunction_(
    'demo_lib',
    [ 'local ' + Blockly.Lua.FUNCTION_NAME_PLACEHOLDER_ + '=require "demo_lib"\n function touch(x,y) '+Blockly.Lua.FUNCTION_NAME_PLACEHOLDER_ + '.touch(x,y) end']);

  var code = demo_lib+".save_all()"

  return code;
};

Blockly.Blocks['tetris_createtemplate'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create tetris template with:");
    this.appendStatementInput("controller")
        .setCheck(null)
        .appendField("controller");
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(135);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['tetris_createtemplate'] = function(block) {
  var statements_controller = Blockly.Lua.statementToCode(block, 'controller');
  
  var tetris_lib = Blockly.Lua.provideFunction_(
    'tetris_lib',
    [ 'local ' + Blockly.Lua.FUNCTION_NAME_PLACEHOLDER_ + '=require "tetris_lib"\n ']);


  var code = tetris_lib+'.create_template(function(joint)'+statements_controller+' end)\n';
  return code;
};

Blockly.Blocks['tetris_getjoint'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("get joint")
        .appendField(new Blockly.FieldDropdown([ 

          ["nose","1"],
          ["left eye inner","2"],
          ["left eye","3"],
          ["left eye outer","4"],
          ["right eye inner","5"],
          ["right eye","6"],
          ["right eye outer","7"],
          ["left ear","8"],
          ["right ear","9"],
          ["mouth left","10"],
          ["mouth right","11"],
          ["left shoulder","12"],
          ["right shoulder","13"],
          ["left elbow","14"],
          ["right elbow","15"],
          ["left wrist","16"],
          ["right wrist","17"],
          ["left pinky","18"],
          ["right pinky","19"],
          ["left index","20"],
          ["right index","21"],
          ["left thumb","22"],
          ["right thumb","23"],
          ["left hip","24"],
          ["right hip","25"],
          ["left knee","26"],
          ["right knee","27"],
          ["left ankle","28"],
          ["right ankle","29"],
          ["left heel","30"],
          ["right heel","31"],
          ["left foot index","32"],
          ["right foot index","33"]

          ]), "joint")
        .appendField(new Blockly.FieldDropdown([["x","x"], ["y","y"], ["z","z"]]), "dir");
    this.setOutput(true, null);
    this.setColour(135);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['tetris_getjoint'] = function(block) {
  var dropdown_joint = block.getFieldValue('joint');
  var dropdown_dir = block.getFieldValue('dir');
  // TODO: Assemble Lua into code variable.
  var code = 'joint['+dropdown_joint+'].'+dropdown_dir;
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['tetris_move'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("move")
        .appendField(new Blockly.FieldDropdown([["left","left"], ["right","right"], ["rotate","rotate"], ["down","down"]]), "joint");
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(135);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['tetris_move'] = function(block) {
  var dropdown_joint = block.getFieldValue('joint');

    var tetris_lib = Blockly.Lua.provideFunction_(
    'tetris_lib',
    [ 'local ' + Blockly.Lua.FUNCTION_NAME_PLACEHOLDER_ + '=require "tetris_lib"\n ']);


  var code = tetris_lib+'.left_move_'+dropdown_joint+'()\n';
  return code;
};


