local demo_lib=require "demo_lib"
 function touch(x,y) demo_lib.touch(x,y) end


function init()
  first_page = demo_lib.create_info_page({field1_array={field_type="txt",title="id patient"},field2_array={field_type="txt",title="patient age"},field3_array={field_type="txt",title="field 3"},field4_array={field_type="txt",title="field 4"},field10_array={field_type="txt",title="field 10"},function_next=function()   demo_lib.show(second_page)
    demo_lib.play_sound(consegna,'speaking')
   end,button_txt='next',color='#ffffff'})

  second_page = demo_lib.create_sequence_page({main_image='ITEM 3 A.png',option1_image='ITEM 3A_A.PNG',option2_image='ITEM 3A_B.PNG',option3_image='ITEM 3A_C.PNG',option4_image='ITEM 3A_D.PNG',option5_image='ITEM 3A_E.PNG',option6_image='ITEM 3A_F.PNG',option7_image='ITEM 3A_G.PNG',option8_image='ITEM 3A_H.PNG',option1_correct=false,option2_correct=false,option3_correct=false,option4_correct=false,option5_correct=false,option6_correct=true,option7_correct=false,option8_correct=false,function_correct=function()   demo_lib.play_sound(passaggio,'wow')
    demo_lib.save_all() end,function_wrong=function()   demo_lib.play_sound(suggerimento,'speaking')
   end,shuffle=true,color='#ffffff'})

  demo_lib.show(first_page)
  demo_lib.create_avatar('robin_silent.webm','robin_alone.webm','robin_speaking.webm','robin_wow.webm')
  demo_lib.face_detect({function_attentive=function()  end,function_distracted=function()   demo_lib.play_sound(richiamo,'alone')
   end,sec=1})
  consegna = demo_lib.load_sound('3a_consegna.wav')

  passaggio = demo_lib.load_sound('3a_passaggioproblema_4_1.wav')

  suggerimento = demo_lib.load_sound('3a_suggerimento_1.wav')

  richiamo = demo_lib.load_sound('richiamo_1.wav')

 end
