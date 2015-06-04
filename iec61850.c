#include "Python.h"
#include "iec61850_client.h"
#include "string.h"
#include "socket.h"
#include "thread.h"

static IedConnection con;
static IedClientError error;


/**********************************************
  Cria conexao com o servidor de dados IEC61850
***********************************************/
static PyObject* createConnection(PyObject* self, PyObject *args)
{
  char* hostname;
  int tcpPort;
  int st;  

  con = IedConnection_create();

  if (!PyArg_ParseTuple(args, "si", &hostname, &tcpPort))
    {
      return NULL;
    }
  
  IedConnection_connect(con, &error, hostname, tcpPort);
  
  if (error == IED_ERROR_OK)
    {
      st = 0;
    }
  else
    {
      st = 1;
    }
  return Py_BuildValue("i", st);
}

static char createConnection_docs[] = "createConnection(string host, int port) : cria uma conexao com um servidor iec61850 via mms\n";

/***********************************************
  Fecha conexao com o servidor de dados IEC61850
************************************************/
static PyObject* closeConnection( PyObject* self)
{
  int st;
  
  if(error == IED_ERROR_OK)
    {
      IedConnection_destroy(con);
      st = 0;
      
    }else
    {
      st = 1;
      
    }
  
  return Py_BuildValue("i", st);
}

static char closeConnection_docs[] = "closeConnection() : encerra uma conexao com um servidor iec61850 via mms\n";

/************************************************
  Le um valor Float de servidor de dados IEC61850
*************************************************/
static PyObject* readMmsFloatValue(PyObject* self, PyObject *args)
{
  char* path;
  char* FC;
  MmsValue* value;
  
  if (!PyArg_ParseTuple(args, "ss", &path, &FC))
    {
      return NULL;
    }
    
  if (!strcmp(FC,"ST"))
    {
      value = IedConnection_readObject(con, &error, path, ST);
    }else if (!strcmp(FC,"MX"))
    {
      value = IedConnection_readObject(con, &error, path, MX);
    }else if (!strcmp(FC,"CO"))
    {
      value = IedConnection_readObject(con, &error, path, CO);
    }else if (!strcmp(FC,"CF"))
    {
      value = IedConnection_readObject(con, &error, path, CF);
    }else if (!strcmp(FC,"DC"))
    {
      value = IedConnection_readObject(con, &error, path, DC);
    }else if(!strcmp(FC,"EX"))
    {
      value = IedConnection_readObject(con, &error, path, EX);
    }else
    {
      return NULL;
    }
    
  if ( value != NULL)
    {
      float fval = MmsValue_toFloat(value);
      MmsValue_delete(value);
      return Py_BuildValue("f", fval);
    }else
    {
      return NULL;
    }
}

static char readMmsFloatValue_docs[] = "readMmsFloatValue(string path, string FC) : Le um valor do tipo float de um servidor iec61850 via mms\n";

/************************************************
  Escreve um valor Float em um servidor de dados IEC61850
*************************************************/
static PyObject* writeMmsFloatValue(PyObject* self, PyObject *args)
{  
  char* path;
  char* FC;
  float fvalue;
  MmsValue* value;
  
  if (!PyArg_ParseTuple(args, "ssf", &path, &FC, &fvalue))
    {
      return NULL;
    }

  value = MmsValue_newFloat(fvalue);
    
  if (!strcmp(FC,"ST"))
    {
      IedConnection_writeObject(con, &error, path, ST, value);
    }else if (!strcmp(FC,"MX"))
    {
      IedConnection_writeObject(con, &error, path, MX, value);
    }else if (!strcmp(FC,"CO"))
    {
      IedConnection_writeObject(con, &error, path, CO, value);
    }else if (!strcmp(FC,"CF"))
    {
      IedConnection_writeObject(con, &error, path, CF, value);
    }else if (!strcmp(FC,"DC"))
    {
      IedConnection_writeObject(con, &error, path, DC, value);
    }else if(!strcmp(FC,"EX"))
    {
      IedConnection_writeObject(con, &error, path, EX, value);
    }else
    {
      return NULL;
    }
  
  MmsValue_delete(value);

  if (error == IED_ERROR_OK)
    {
      return Py_BuildValue("i", 0);
    }
  else
    {
      return Py_BuildValue("i", 1);
    }
}

static char writeMmsFloatValue_docs[] = "writeMmsFloatValue_docs(): Escreve um valor Float em um atributo de dado de um servidor iec61850 via mms";


/************************************************
  Le um valor de Estado de um servidor de dados IEC61850
*************************************************/
static PyObject* readMmsStatusValue(PyObject* self, PyObject *args){
  char* path;
  char* FC;
  MmsValue* value;
  
  if (!PyArg_ParseTuple(args, "ss", &path, &FC))
    {
      return NULL;
    }
    
  if (!strcmp(FC,"ST"))
    {
      value = IedConnection_readObject(con, &error, path, ST);
    }else if (!strcmp(FC,"MX"))
    {
      value = IedConnection_readObject(con, &error, path, MX);
    }else if (!strcmp(FC,"CO"))
    {
      value = IedConnection_readObject(con, &error, path, CO);
    }else if (!strcmp(FC,"CF"))
    {
      value = IedConnection_readObject(con, &error, path, CF);
    }else if (!strcmp(FC,"DC"))
    {
      value = IedConnection_readObject(con, &error, path, DC);
    }else if(!strcmp(FC,"EX"))
    {
      value = IedConnection_readObject(con, &error, path, EX);
    }else
    {
      return NULL;
    }
    
  if ( value != NULL)
    {
      int32_t stVal = MmsValue_getBitStringAsInteger(value);
      MmsValue_delete(value);
      return Py_BuildValue("i", stVal);
    }else
    {
      return NULL;
    }
}

static char readMmsStatusValue_docs[] = "readMmsStatusValue_docs(): Le um valor de estado de um atributo de dado de um servidor iec 61850 via mms";

static PyObject* operate(PyObject* self, PyObject *args){
  
  int value;
  char* dataAttributeRef;
  MmsValue* ctlVal;
  MmsValue* stVal;

  if (!PyArg_ParseTuple(args, "si", &dataAttributeRef, &value))
    {
      return NULL;
    }

  printf("%s\n", dataAttributeRef);
  printf("%d\n", value);
  //printf("valor: %i\n", value);

  ControlObjectClient control = ControlObjectClient_create((char*) dataAttributeRef, con);
  
  if (value == 1){
    ctlVal = (MmsValue*) MmsValue_newBoolean(true);
  }else{
    ctlVal = (MmsValue*) MmsValue_newBoolean(false);
  }
  
  ControlObjectClient_operate(control, ctlVal, 0);
  /*
  MmsValue_delete(ctlVal);

  ControlObjectClient_destroy(control);

  stVal = IedConnection_readObject(con, &error, strcat(dataAttributeRef, ".stVal"), ST);
  */
  if (error == IED_ERROR_OK){
    return Py_BuildValue("i", 0);
    //bool state = MmsValue_getBoolean(stVal);

    /*
    if (state == value){
      return Py_BuildValue("i", 0);
    }else{
      return Py_BuildValue("i", 1);
    }
    */
  }else{
    return Py_BuildValue("i", 1);
  }

}

static char operate_docs[] = "operate(dataAttributeReference, valor) : envia comando para servidor IEC61850 no modo operate\n";

/**********************************************
  Retorna lista com os Dispositivos Lógicos do 
  servidor de dados IEC61850
***********************************************/
static PyObject* getLogicalDeviceList(PyObject* self)
{
  LinkedList deviceList = IedConnection_getLogicalDeviceList(con, &error);
 
  PyObject* logicalDeviceList = PyList_New(LinkedList_size(deviceList));

  LinkedList device = LinkedList_getNext(deviceList);
 
  int cont = 0;
  
  while (device != NULL)
    {
      PyList_SET_ITEM(logicalDeviceList, 
                      cont, 
                      PyString_FromStringAndSize((char*) device->data, 
                                                 strlen((char*) device->data)
                                                 )
                      );
      device = LinkedList_getNext(device);
      cont++;
    }  

  return logicalDeviceList;
}

static char getLogicalDeviceList_docs[] = "getLogicalDeviceList() : Retorna uma lista com o nome dos Dispositivos Logicos de um servidor iec61850 via mms\n";

/***********************************************
  Retorna uma lista com os nomes dos Nos Logicos 
  de um Dispositivo Logico associado a um servidor 
  de dados IEC61850
************************************************/
static PyObject* getLogicalDeviceDirectory(PyObject* self, PyObject *args)
{
  char* logicalDevice;

  if(!PyArg_ParseTuple(args, "s", &logicalDevice))
    {
      return NULL;
    }
  
  LinkedList logicalNodes = IedConnection_getLogicalDeviceDirectory(con, 
                                                                    &error, 
                                                                    logicalDevice);
  
  PyObject* logicalNodesList = PyList_New(LinkedList_size(logicalNodes));
  
  int cont = 0;
  
  if (error == IED_ERROR_OK)
    {
      logicalNodes = LinkedList_getNext(logicalNodes);
      while (logicalNodes != NULL)
	      {
	         PyList_SET_ITEM(logicalNodesList, 
                           cont, 
                           PyString_FromStringAndSize((char*) logicalNodes->data, 
                                                      strlen((char*) logicalNodes->data)
                                                      )
                           );
	         logicalNodes = LinkedList_getNext(logicalNodes);
	         cont++;
	      }
        return logicalNodesList;
    }else
    {
      return NULL;
    }
}

static char getLogicalDeviceDirectory_docs[] = "getLogicalDeviceDirectory() : Retorna os Nos Logicos de um Dispositivo Logico de um servidor iec61850 via mms\n";


static PyObject* getLogicalNodeDirectory( PyObject* self, PyObject *args)
{
  char* logicalNodeName;
  int cont;
  LinkedList dataObjects;
  LinkedList dataObject;
  PyObject* dataObjectsList;

  if (!PyArg_ParseTuple(args, "s", &logicalNodeName))
  {
    return NULL;
  }

  dataObjects = IedConnection_getLogicalNodeDirectory(con, 
                                                      &error,
                                                      logicalNodeName,
                                                      ACSI_CLASS_DATA_OBJECT);
  
  dataObjectsList = PyList_New(LinkedList_size(dataObjects));
  
  dataObject = LinkedList_getNext(dataObjects);
  cont = 0;

  if (error == IED_ERROR_OK){
    while (dataObject != NULL){
      PyList_SET_ITEM(dataObjectsList, 
                      cont,
                      PyString_FromStringAndSize((char*) dataObject->data,
                                                 strlen((char*) dataObject->data)
                      )
      );
      dataObject = LinkedList_getNext(dataObject);
      cont++;
    }
    return dataObjectsList;
  }else{
    return NULL;
  }
}

static char getLogicalNodeDirectory_docs[] = "getLogicalNodeDirectory(str LNRef) : retorna os objetos de dados presentes em um No Logico de um IED\n";

static PyObject* getDataDirectory(PyObject* self, PyObject *args){
  
  char* dataObjectName;
  int cont;
  LinkedList dataAttributes;
  LinkedList dataAttribute;
  PyObject* dataAttributeList;

  if (!PyArg_ParseTuple(args, "s", &dataObjectName)){
    return NULL;
  }

  dataAttributes = IedConnection_getDataDirectory(con,
                                                  &error,
                                                  dataObjectName);

  dataAttributeList = PyList_New(LinkedList_size(dataAttributes));  

  dataAttribute = LinkedList_getNext(dataAttributes);

  cont = 0;

  if (error == IED_ERROR_OK){
    while (dataAttribute != NULL){

     PyList_SET_ITEM(dataAttributeList, 
                     cont,
                     PyString_FromStringAndSize((char*) dataAttribute->data,
                                                 strlen((char*) dataAttribute->data)
                      )
      );
      dataAttribute = LinkedList_getNext(dataAttribute);
      cont++; 
    }
    return dataAttributeList;
  }else{
    return NULL;
  }
}

static char getDataDirectory_docs[] = "getDataDirectory(str DORef) : Retorna os atributos de dados presentes em um IED";

/***********************************************
  Retorna um dicionario com os valores presentes
  em um DataSet associado a um servidor de dados
  IEC61850
************************************************/
static PyObject* readDataSetValues( PyObject* self, PyObject *args)
{
  char* dataSetName;
  char* atributeType;
  ClientDataSet dataSet;
  MmsValue* dataSetMmsValue;
  PyObject* dataSetDict;
  PyObject* dictKey;
  LinkedList dataSetLinkedList;
  LinkedList currentDataSetAtribute;
  
  int cont;

  if (!PyArg_ParseTuple(args, "s", &dataSetName))
    {
      return NULL;
    }
  
  //Obtencao dos nomes de referencia dos atributos de dados e de seus respectivos valores
  dataSetLinkedList = IedConnection_getDataSetDirectory(con, &error, dataSetName, NULL);

  dataSet = IedConnection_readDataSetValues(con, &error, dataSetName, NULL);
  
  dataSetMmsValue = ClientDataSet_getValues(dataSet);
  
  dataSetDict = PyDict_New();
  
  if (error == IED_ERROR_OK)
    {
      currentDataSetAtribute = LinkedList_getNext(dataSetLinkedList);
      cont = 0;
      while(currentDataSetAtribute != NULL)
    	 {
    	  dictKey = PyString_FromStringAndSize((char*) currentDataSetAtribute->data, 
                                             strlen((char*) currentDataSetAtribute->data)
                                             );
    	  atributeType = (char*)MmsValue_getTypeString(
                                                     MmsValue_getElement(
                                                                         dataSetMmsValue, 
                                                                         cont)
                                                     );
    	  printf("Tipo do objeto: %s", atributeType);
    	  
    	  if (!strcmp(atributeType, "boolean"))
    	    {
    	      if ( MmsValue_getBoolean(MmsValue_getElement(dataSetMmsValue, cont)))
          		{
          		  PyDict_SetItem(dataSetDict, dictKey, Py_True);
          		}else
          		{
          		  PyDict_SetItem(dataSetDict, dictKey, Py_False);
          		}
    	    }
    	  else if(!strcmp(atributeType, "integer"))
    	      {
    		      PyDict_SetItem(dataSetDict, 
                             dictKey, 
                             Py_BuildValue("i", 
                                           MmsValue_toInt32(
                                                            MmsValue_getElement(dataSetMmsValue, cont)
                                                            )
                                           )
                             );
    	      }
    	  else if(!strcmp(atributeType, "bit-string"))
    	    {
    	      PyDict_SetItem(dataSetDict, 
                           dictKey, 
                           Py_BuildValue("i", 
                                         MmsValue_getBitStringAsInteger(
                                                                        MmsValue_getElement(
                                                                          dataSetMmsValue, 
                                                                          cont)
                                                                        )
                                         )
                           );
    	    }else
    	    {
    	      PyDict_SetItem(dataSetDict, 
                           dictKey, 
                           Py_BuildValue("s", atributeType)
                           );
    	    }
    	  
    	    
    	    currentDataSetAtribute = LinkedList_getNext(currentDataSetAtribute);
    	    cont++;
    	}// fim do while
      
      return dataSetDict;
      
    }// fim do if error == IED_ERROR_OK
  else
    {
      return NULL;
    }//fim do else
}// fim do metodo

static char readDataSetValues_docs[] = "readDataSetValues(dataSetName) : retorna um dicionario com os nomes e valores dos atributos de dados de um dataSet num servidor iec61850 via mms";

/***********************************************
  Retorna uma lista com os nomes dos atributos 
  de dados encapsulados em um DataSet associado
  a um servidor de dados IEC61850
************************************************/
static PyObject* getDataSetDirectory(PyObject* self, PyObject *args)
{
  char* path;
  LinkedList dataSetAtributes;
  LinkedList atribute;
  PyObject* dataSetList;
  int cont;
  
  if( !PyArg_ParseTuple(args, "s", &path))
    {
      return NULL;
    }
  dataSetAtributes = IedConnection_getDataSetDirectory(con, &error, path, NULL);
  
  if (error == IED_ERROR_OK)
    {
      dataSetList = PyList_New(LinkedList_size(dataSetAtributes));
      atribute = LinkedList_getNext(dataSetAtributes);
      cont = 0;
      
      while(atribute != NULL)
      {
        PyList_SET_ITEM(dataSetList, cont, PyString_FromStringAndSize((char*) atribute->data, strlen((char*) atribute->data)));
        atribute = LinkedList_getNext(atribute);
        cont++;
      }
      return dataSetList;
    }else
    {
      return NULL;
    }
}

static char getDataSetDirectory_docs[] = "getDataSetDirectory() : Retorna os atributos de dados presentes em um dataSet de um servidor iec61850 via mms";

void
reportCallbackFunction(void* parameter, ClientReport report)
{
    MmsValue* dataSetValues = ClientReport_getDataSetValues(report);

    printf("received report for %s\n", ClientReport_getRcbReference(report));

    int i = 1;
    
    for (i = 0; i < 4; i++) {
        ReasonForInclusion reason = ClientReport_getReasonForInclusion(report, i);

        if (reason != REASON_NOT_INCLUDED)
	  {
	    Socket soc = TcpSocket_create();
	    Socket_connect(soc, "localhost", 8000);
	    char* msg = "lucas";
	    Socket_write(soc, (uint8_t*)msg, strlen(msg));
	  }//fim do if
    }//fim do for
}

static PyObject* receiveReport(PyObject* self, PyObject *args)
{
  char* reportReference;
  //  ClientDataSet dataSet;
  ClientReportControlBlock rcb;
  
  if (!PyArg_ParseTuple(args, "s", &reportReference))
  {
    return NULL;
  }
  
  rcb = IedConnection_getRCBValues(con, &error, reportReference, NULL);
  if (error == IED_ERROR_OK){
    printf("%s\n", reportReference);
    printf("%s\n", ClientReportControlBlock_getRptId(rcb));
  }else{
    printf("Ocoreu um erro ao ler o ReportControl Block!\n");
  }
  /*
  IedConnection_installReportHandler(con, reportReference, ClientReportControlBlock_getRptId(rcb),reportCallbackFunction, NULL);
  
  ClientReportControlBlock_setTrgOps(rcb, TRG_OPT_DATA_UPDATE | TRG_OPT_INTEGRITY | TRG_OPT_GI);
  
  ClientReportControlBlock_setRptEna(rcb, true);
  
  ClientReportControlBlock_setIntgPd(rcb, 5000);
  
  IedConnection_setRCBValues(con, &error, rcb, RCB_ELEMENT_RPT_ENA | RCB_ELEMENT_TRG_OPS | RCB_ELEMENT_INTG_PD, true);

  Thread_sleep(10000);
  */
  return Py_BuildValue("i", 0);
}

static char receiveReport_docs[] = "receiveReport(): recebe relatorios de um servidor iec61850 via mms";
 
static PyMethodDef iec61850_funcs[] = 
  {
    {"createConnection", (PyCFunction)createConnection, METH_VARARGS, createConnection_docs },
    {"closeConnection", (PyCFunction)closeConnection, METH_NOARGS, closeConnection_docs },
    {"readMmsFloatValue", (PyCFunction)readMmsFloatValue, METH_VARARGS, readMmsFloatValue_docs },
    {"readMmsStatusValue", (PyCFunction)readMmsStatusValue, METH_VARARGS, readMmsStatusValue_docs },
    {"operate", (PyCFunction)operate, METH_VARARGS, operate_docs},
    {"getLogicalDeviceList", (PyCFunction)getLogicalDeviceList, METH_NOARGS, getLogicalDeviceList_docs },
    {"getLogicalDeviceDirectory", (PyCFunction)getLogicalDeviceDirectory, METH_VARARGS, getLogicalDeviceDirectory_docs },
    {"getLogicalNodeDirectory", (PyCFunction)getLogicalNodeDirectory, METH_VARARGS, getLogicalNodeDirectory_docs},
    {"getDataDirectory", (PyCFunction)getDataDirectory, METH_VARARGS, getDataDirectory_docs},
    {"writeMmsFloatValue", (PyCFunction)writeMmsFloatValue, METH_VARARGS, writeMmsFloatValue_docs },
    {"readDataSetValues", (PyCFunction)readDataSetValues, METH_VARARGS, readDataSetValues_docs },    
    {"getDataSetDirectory", (PyCFunction)getDataSetDirectory, METH_VARARGS, getDataSetDirectory_docs },
    {"receiveReport", (PyCFunction)receiveReport, METH_VARARGS, receiveReport_docs},
    
    {NULL}
  };

void initiec61850(void)
{
  Py_InitModule3("iec61850", iec61850_funcs, "modulo Python iec61850");
  
}
