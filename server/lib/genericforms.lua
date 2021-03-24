function create_form()
    label_titolo=proteo.gui.newLabel('label_titolo','Richiesta di Contatto','Helvetica',30,"#9a428d","#fcfcfc",proteo.gui.LabelAlignment.Left,0,0,300,40)
    label_sottotitolo1=proteo.gui.newLabel('label_sottotitolo1','Compila la scheda per effettuare senza impegno la prenotazione di contatto con CuoreMenteLab','Helvetica',25,"#2b9dd5","Clear",proteo.gui.LabelAlignment.Left,0,0,300,40)
    label_sottotitolo2=proteo.gui.newLabel('label_sottotitolo2','Sarai ricontattato e ti saranno fornite tutte le informazioni di cui necessiti.','Helvetica',25,"#2b9dd5","Clear",proteo.gui.LabelAlignment.Left,0,0,300,40)
    label_sottotitolo3=proteo.gui.newLabel('label_sottotitolo3','Le ricordiamo che il presente modulo sarà inviato a tutti gli indirizzi e-mail inseriti all\'interno del modulo stesso','Helvetica',15,"#2b9dd5","Clear",proteo.gui.LabelAlignment.Left,0,0,300,40)

    label_nome=proteo.gui.newLabel('label_nome','Nome richiedente','Helvetica',15,"#757575","Clear",proteo.gui.LabelAlignment.Left,0,0,300,40)
    label_cognome=proteo.gui.newLabel('label_cognome','Cognome richiedente','Helvetica',15,"#757575","Clear",proteo.gui.LabelAlignment.Left,0,0,300,40)
    txt_nome=proteo.gui.newTextField('txt_nome','','Helvetica',15,'#757575','White',0,0,300,30,'')
    txt_cognome=proteo.gui.newTextField('txt_cognome','','Helvetica',15,'#757575','White',0,0,300,30,'')

    btn_next=proteo.gui.newButton('btn_next','Successivo','Helvetica',15,"black","#eeeeee",1,"clear",true,0,0,150,50,"EnterNext")
    
    newForm=GridForm('newForm',20,20)
    newForm.backgroundColor="#fcfcfc"
    newForm:addControl(2,1,proteo.gui.ControlAnchor.CenterLeft,label_titolo)
    newForm:addControl(2,2,proteo.gui.ControlAnchor.CenterLeft,label_sottotitolo1)
    newForm:addControl(2,3,proteo.gui.ControlAnchor.CenterLeft,label_sottotitolo2)
    newForm:addControl(2,4,proteo.gui.ControlAnchor.CenterLeft,label_sottotitolo3)

    newForm:addControl(2,6,proteo.gui.ControlAnchor.CenterLeft,label_nome)
    newForm:addControl(10,6,proteo.gui.ControlAnchor.CenterLeft,label_cognome)
    newForm:addControl(2,7,proteo.gui.ControlAnchor.CenterLeft,txt_nome)
    newForm:addControl(10,7,proteo.gui.ControlAnchor.CenterLeft,txt_cognome)

    newForm:addControl(3,18,proteo.gui.ControlAnchor.Center,btn_next)

    newForm:updatePosition()

    return newForm
end