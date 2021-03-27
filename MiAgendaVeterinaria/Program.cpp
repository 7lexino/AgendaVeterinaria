#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <string>
#include <fstream>
#include "Identificadores.h"
#include <CommCtrl.h> //Agrega código para utilizar los DateTimePicker y ListView
#include <ctime>

using namespace std;

enum TipoActualizacionAgenda{SIMPLE,AGREGAR,EDITAR,ELIMINAR};

enum Agenda{
	REGISTROSPORPAGINA = 10
};

//Estructuras
struct Fecha {
	string fechaCompleta;
	byte dia;
	byte mes;
	unsigned int ano;
};

struct Hora {
	string horaCompleta;
	byte hora;
	byte minuto;
};

struct DatosCita {
	unsigned int NumCita;
	Fecha fechaCita;
	Hora horaCita;
	float costo;
	string motivoConsulta;
};

struct DatosCliente {
	string nombre;
	string apellido;
	string telefono;
	string nombreCompleto() {
		return nombre + " " + apellido;
	}
};

struct DatosMascota {
	string nombre;
	string especie;
	bool esMacho;
};

struct Cita {
	DatosCita datosCita;
	DatosCliente datosCliente;
	DatosMascota datosMascota;
	Cita *ant, *sig;
}*origen, *aux, *ultimo;

struct Doctor {
	string cedula, nombre, apellido, telefono, correo, rutaFoto, rutaFotoTemp;
	string NombreCompleto() {
		return nombre + " " + apellido;
	}
} doctor;

//Procedimientos de ventanas
LRESULT CALLBACK procLogin(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK procPerfil(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK procCitaNueva(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK procConfig(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK procAgenda(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK procResumenCita(HWND, UINT, WPARAM, LPARAM);

//Procedimientos de controles
LRESULT CALLBACK procTxtContra(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK procTxtCedula(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK procTxtNombre(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK procTxtApellido(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK procTxtTelefono(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK procGrpCita(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK procGrpCliente(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK procGrpMascota(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK procRegCitaFechaCita(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK procRegCitaHoraCita(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK procRegCitaCostoCita(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK procRegCitaMotivoCita(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK procRegCitaNombreCliente(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK procRegCitaApellidoCliente(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK procRegCitaTelefonoCliente(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK procRegCitaNombreMascota(HWND, UINT, WPARAM, LPARAM);

//WNDPROC para detectar teclas
WNDPROC wndprocTxtCedula, wndprocTxtNombre, wndprocTxtApellido,
wndprocTxtTelefono, wndprocTxtCorreo, wndprocTxtContra,
wndprocRegCitaFechaCita, wndprocRegCitaHoraCita, wndprocRegCitaCostoCita,
wndprocRegCitaMotivoCita, wndprocRegCitaNombreCliente, wndprocRegCitaApellidoCliente,
wndprocRegCitaTelefonoCliente, wndprocRegCitaNombreMascota, wndprocGrpCita,
wndprocGrpCliente, wndprocGrpMascota;

//Macros
bool RegistrarClases();
void InsertarControlesLogin(HWND);
void InsertarControlesPerfil(HWND);
void InsertarControlesCitaNueva(HWND);
void InsertarControlesAgenda(HWND);
void InsertarControlesResumenCita(HWND);
void MostrarVentanaPrincipal();
void AgregarCita();
void ActualizarCita();
void GuardarTodo();
void CargarDatosCita();
void CargarDatosDoctor();
void LimpiarCamposRegistroCita();
void ConfirmarSalir();
void InsertarAgenda(byte, byte, string, HWND);

//Funciones
int CenterToScreenX(HWND);
int CenterToScreenY(HWND);
bool ValidarLogin(HWND);
bool ValidarCita();
bool EsNumero(string);
bool EsMoneda(string);
bool EsTexto(string);
void DesglozarFecha(string, byte *, byte *, unsigned int *);
void DesglozarTiempo(string, byte *, byte *);
string TraerNumNuevaCita();
int CompararFecha(Fecha, Fecha);
int CompararHora(Hora, Hora);
Cita *BuscarCitaPorId(int);

/*---------------Variables Globales-----------------*/

int citas;

//WINDOWS
HWND hwndWndLogin, hwndWndPerfil, hwndWndCitaNueva, hwndWndConfig, hwndWndAgenda, hwndWndResumenCita;

//HINSTANCE
HINSTANCE hInst;

namespace Mensajes {

	int Error() {
		return MessageBox(GetActiveWindow(), "Se ha producido un error, favor de comunicarse con el administrador del sistema.", "ERROR", MB_ICONERROR | MB_OK);
	}

	int OperacionNoValida(string mensaje) {
		return MessageBox(GetActiveWindow(), mensaje.c_str(), "Operación no válida", MB_ICONEXCLAMATION | MB_OK);
	}

	int OperacionExitosa(string mensaje) {
		return MessageBox(GetActiveWindow(), mensaje.c_str(), "Operación exitosa", MB_ICONINFORMATION | MB_OK);
	}

	int Confirmacion(string mensaje) {
		return MessageBox(GetActiveWindow(), mensaje.c_str(), "Confirmación", MB_ICONQUESTION | MB_YESNO | MB_APPLMODAL);
	}
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	//InitCommonControls();
	hInst = hInstance; //Hacemos global la instancia de la aplicación
	if (!RegistrarClases()) {
		Mensajes::Error();
		return 0;
	}

	//hwndWndLogin = CreateWindowEx( //Creamos nuestra ventana
	//	0,
	//	"wcWndLogin",
	//	"Iniciar sesión",
	//	WS_POPUP,
	//	0,
	//	0,
	//	200,
	//	150,
	//	HWND_DESKTOP,
	//	NULL,
	//	hInstance,
	//	NULL
	//);

	////Centramos nuestra ventana y la mostramos
	//SetWindowPos(hwndWndLogin, 0, CenterToScreenX(hwndWndLogin), CenterToScreenY(hwndWndLogin), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	//ShowWindow(hwndWndLogin, SW_SHOW);

	MostrarVentanaPrincipal();

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	//Corazón de nuestra aplicación
	while (TRUE == GetMessage(&msg, 0, 0, 0)) {
		if (IsDialogMessage(hwndWndAgenda, &msg)) continue;
		if (IsDialogMessage(hwndWndLogin, &msg)) continue;
		if (IsDialogMessage(hwndWndPerfil, &msg)) continue;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

void InsertarControlesLogin(HWND hWnd) {
	CreateWindowEx(0, "STATIC", "Usuario: ", WS_CHILD | WS_VISIBLE,
		25, 25, 55, 15, hWnd,
		(HMENU)LBL_USUARIO, hInst, NULL);

	HWND hwndTxtUsuario = CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL,
		25, 45, 150, 20, hWnd, (HMENU)TXT_USUARIO, hInst, NULL);
	SendMessage(hwndTxtUsuario, EM_LIMITTEXT, WPARAM(20), 0); //Ponemos un límite para escribir caracteres
	SetFocus(hwndTxtUsuario);

	CreateWindowEx(0, "STATIC", "Contraseña: ",
		WS_CHILD | WS_VISIBLE,
		25, 70, 78, 15, hWnd,
		(HMENU)LBL_PASS, hInst, NULL);

	HWND hwndTxtContra = CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL | ES_PASSWORD,
		25, 90, 150, 20, hWnd, (HMENU)TXT_PASS, hInst, NULL);
	SendMessage(hwndTxtContra, EM_LIMITTEXT, WPARAM(25), 0); //Ponemos un límite para escribir caracteres
	wndprocTxtContra = (WNDPROC)SetWindowLongPtr(hwndTxtContra, GWL_WNDPROC, (LONG_PTR)procTxtContra);

	CreateWindowEx(0, "BUTTON", "Entrar", WS_CHILD | WS_VISIBLE | WS_BORDER,
		60, 115, 80, 25, hWnd, (HMENU)BTN_ENTRAR, hInst, NULL);
}

void InsertarControlesPerfil(HWND hWnd) {

	HMENU hmContenedorMenuPerfil;
	hmContenedorMenuPerfil = CreateMenu();
	
	AppendMenu(hmContenedorMenuPerfil, MF_STRING | MF_POPUP, MENUPERFIL_EDITAR, "Editar perfil");

	SetMenu(hWnd, hmContenedorMenuPerfil);

	//--------------- Insertar resto de controles ------------------//
	CreateWindowEx(0, "STATIC", "ERROR",
		WS_CHILD | WS_BORDER | SS_CENTER,
		0, 0, 505, 18, hWnd, (HMENU)LBL_INFODOCTOR_ERROR, hInst, NULL);

	CreateWindowEx(0, "STATIC", "Cédula: ",
		WS_VISIBLE | WS_CHILD,
		20, 30, 100, 20, hWnd, NULL, hInst, NULL);

	HWND hwndTxtCedula = CreateWindowEx(0, "EDIT", "",
		WS_VISIBLE | WS_CHILD | WS_BORDER | WS_DISABLED,
		100, 30, 125, 20, hWnd, (HMENU)TXT_CEDULADOCTOR, hInst, NULL);
	wndprocTxtCedula = (WNDPROC)SetWindowLongPtr(hwndTxtCedula, GWL_WNDPROC, (LONG_PTR)procTxtCedula);

	CreateWindowEx(0, "STATIC", "Nombre: ",
		WS_VISIBLE | WS_CHILD,
		20, 60, 100, 20, hWnd, NULL, hInst, NULL);

	HWND hwndTxtNombre = CreateWindowEx(0, "EDIT", "",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_DISABLED | ES_UPPERCASE,
		100, 60, 200, 20, hWnd, (HMENU)TXT_NOMBREDOCTOR, hInst, NULL);
	wndprocTxtNombre = (WNDPROC)SetWindowLongPtr(hwndTxtNombre, GWL_WNDPROC, (LONG_PTR)procTxtNombre);

	CreateWindowEx(0, "STATIC", "Apellido: ",
		WS_VISIBLE | WS_CHILD,
		20, 90, 100, 20, hWnd, NULL, hInst, NULL);

	HWND hwndTxtApellido = CreateWindowEx(0, "EDIT", "",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_DISABLED | ES_UPPERCASE,
		100, 90, 200, 20, hWnd, (HMENU)TXT_APELLIDODOCTOR, hInst, NULL);
	wndprocTxtApellido = (WNDPROC)SetWindowLongPtr(hwndTxtApellido, GWL_WNDPROC, (LONG_PTR)procTxtApellido);

	CreateWindowEx(0, "STATIC", "Teléfono: ",
		WS_VISIBLE | WS_CHILD,
		20, 120, 100, 20, hWnd, NULL, hInst, NULL);

	HWND hwndTxtTelefono = CreateWindowEx(0, "EDIT", "",
		WS_VISIBLE | WS_CHILD | WS_BORDER | WS_DISABLED,
		100, 120, 100, 20, hWnd, (HMENU)TXT_TELEFONODOCTOR, hInst, NULL);
	wndprocTxtTelefono = (WNDPROC)SetWindowLongPtr(hwndTxtTelefono, GWL_WNDPROC, (LONG_PTR)procTxtTelefono);

	CreateWindowEx(0, "STATIC", "Correo: ",
		WS_VISIBLE | WS_CHILD,
		20, 150, 100, 20, hWnd, NULL, hInst, NULL);

	CreateWindowEx(0, "EDIT", "",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_DISABLED | ES_UPPERCASE,
		100, 150, 200, 20, hWnd, (HMENU)TXT_CORREODOCTOR, hInst, NULL);

	CreateWindowEx(0, "BUTTON", "Elegir foto",
		WS_CHILD | WS_BORDER,
		340, 175, 150, 25, hWnd, (HMENU)BTN_ELEGIRFOTODOCTOR, hInst, NULL);


}

void InsertarControlesCitaNueva(HWND hWnd) {
	//-------------- Insertar menu ---------------//
	HMENU hmContenedor = GetMenu(hWnd);
	hmContenedor = CreateMenu();

	AppendMenu(hmContenedor, MF_STRING | MF_POPUP, NULL, "");
	AppendMenu(hmContenedor, MF_STRING | MF_POPUP, MENU_PERFIL, doctor.NombreCompleto().c_str());

	HBITMAP bitdoctor = (HBITMAP)LoadImage(NULL, doctor.rutaFoto.c_str(), IMAGE_BITMAP, 25, 25, LR_LOADFROMFILE);
	ModifyMenu(hmContenedor, 0, MF_BYPOSITION | MF_BITMAP | MF_STRING, 0, (LPCTSTR)bitdoctor);
	SetMenu(hWnd, hmContenedor);
	//----------------------------------------------------//

	// ------------------ DATOS DE LA CITA ------------------//
	HWND grpDatosCita = CreateWindowEx(0, "BUTTON", "Datos de la cita",
		BS_GROUPBOX | WS_VISIBLE | WS_CHILD | WS_GROUP,
		10, 10, 565, 190, hWnd, (HMENU)GRP_DATOSCITA, hInst, NULL);
	wndprocGrpCita = (WNDPROC)SetWindowLongPtr(grpDatosCita, GWL_WNDPROC, (LONG_PTR)procGrpCita);

	CreateWindowEx(0, "STATIC", "No cita:  ",
		WS_VISIBLE | WS_CHILD,
		10, 30, 100, 20, grpDatosCita, NULL, hInst, NULL);

	HWND hwndTxtNumCita = CreateWindowEx(0, "EDIT", "",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER | WS_DISABLED,
		115, 30, 100, 20, grpDatosCita, (HMENU)TXT_NUMEROCITA, hInst, NULL);
	SetWindowText(hwndTxtNumCita, TraerNumNuevaCita().c_str());

	CreateWindowEx(0, "STATIC", "Fecha de cita:  ",
		WS_VISIBLE | WS_CHILD,
		245, 30, 100, 20, grpDatosCita, NULL, hInst, NULL);

	HWND hwndFechaCita = CreateWindowEx(0, "SysDateTimePick32", "",
		WS_VISIBLE | WS_CHILD | WS_BORDER | DTS_SHOWNONE,
		350, 30, 200, 25, grpDatosCita, (HMENU)DT_FECHACITA, hInst, NULL);
	wndprocRegCitaFechaCita = (WNDPROC)SetWindowLongPtr(hwndFechaCita, GWL_WNDPROC, (LONG_PTR)procRegCitaFechaCita);

	CreateWindowEx(0, "STATIC", "Hora de cita:  ",
		WS_VISIBLE | WS_CHILD,
		10, 60, 100, 20, grpDatosCita, NULL, hInst, NULL);

	HWND dpHoraCita = CreateWindowEx(0, "SysDateTimePick32", "",
		WS_VISIBLE | WS_CHILD | WS_BORDER | DTS_TIMEFORMAT,
		115, 60, 120, 25, grpDatosCita, (HMENU)DT_HORACITA, hInst, NULL);
	//DateTime_SetFormat(dpHoraCita, "'H':'mm'");
	SendMessage(dpHoraCita, DTM_SETFORMAT, 0, (LPARAM)"HH:mm");
	wndprocRegCitaHoraCita = (WNDPROC)SetWindowLongPtr(dpHoraCita, GWL_WNDPROC, (LONG_PTR)procRegCitaHoraCita);

	CreateWindowEx(0, "STATIC", "Costo:  ",
		WS_VISIBLE | WS_CHILD,
		245, 60, 100, 20, grpDatosCita, NULL, hInst, NULL);

	HWND hwndTxtCostoCita = CreateWindowEx(0, "EDIT", "",
		WS_VISIBLE | WS_CHILD | WS_BORDER,
		350, 60, 100, 20, grpDatosCita, (HMENU)TXT_COSTOCITA, hInst, NULL);
	wndprocRegCitaCostoCita = (WNDPROC)SetWindowLongPtr(hwndTxtCostoCita, GWL_WNDPROC, (LONG_PTR)procRegCitaCostoCita);

	CreateWindowEx(0, "STATIC", "Motivo de consulta:  ",
		WS_VISIBLE | WS_CHILD,
		10, 90, 150, 20, grpDatosCita, NULL, hInst, NULL);

	HWND hwndTxtMotivo = CreateWindowEx(0, "EDIT", "",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_UPPERCASE,
		10, 115, 400, 60, grpDatosCita, (HMENU)TXT_MOTIVOCONSULTA, hInst, NULL);
	wndprocRegCitaMotivoCita = (WNDPROC)SetWindowLongPtr(hwndTxtMotivo, GWL_WNDPROC, (LONG_PTR)procRegCitaMotivoCita);
	
	// ------------------ DATOS DEL CLIENTE ------------------//
	HWND grpDatosCliente = CreateWindowEx(0, "BUTTON", "Datos del cliente",
		BS_GROUPBOX | WS_VISIBLE | WS_CHILD | WS_GROUP,
		10, 210, 565, 125, hWnd, (HMENU)GRP_DATOSCLIENTE, hInst, NULL);
	wndprocGrpCliente = (WNDPROC)SetWindowLongPtr(grpDatosCliente, GWL_WNDPROC, (LONG_PTR)procGrpCliente);

	CreateWindowEx(0, "STATIC", "Nombre:  ",
		WS_VISIBLE | WS_CHILD,
		10, 30, 100, 20, grpDatosCliente, NULL, hInst, NULL);

	HWND hwndNombreCliente = CreateWindowEx(0, "EDIT", "",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE,
		115, 30, 250, 20, grpDatosCliente, (HMENU)TXT_NOMBRECLIENTE, hInst, NULL);
	wndprocRegCitaNombreCliente = (WNDPROC)SetWindowLongPtr(hwndNombreCliente, GWL_WNDPROC, (LONG_PTR)procRegCitaNombreCliente);

	CreateWindowEx(0, "STATIC", "Apellido:  ",
		WS_VISIBLE | WS_CHILD,
		10, 60, 100, 20, grpDatosCliente, NULL, hInst, NULL);

	HWND hwndApellidoCliente = CreateWindowEx(0, "EDIT", "",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE,
		115, 60, 250, 20, grpDatosCliente, (HMENU)TXT_APELLIDOCLIENTE, hInst, NULL);
	wndprocRegCitaApellidoCliente = (WNDPROC)SetWindowLongPtr(hwndApellidoCliente, GWL_WNDPROC, (LONG_PTR)procRegCitaApellidoCliente);

	CreateWindowEx(0, "STATIC", "Teléfono:  ",
		WS_VISIBLE | WS_CHILD,
		10, 90, 100, 20, grpDatosCliente, NULL, hInst, NULL);

	HWND hwndTelefonoCliente = CreateWindowEx(0, "EDIT", "",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
		115, 90, 150, 20, grpDatosCliente, (HMENU)TXT_TELEFONOCLIENTE, hInst, NULL);
	wndprocRegCitaTelefonoCliente = (WNDPROC)SetWindowLongPtr(hwndTelefonoCliente, GWL_WNDPROC, (LONG_PTR)procRegCitaTelefonoCliente);

	// ------------------ DATOS DE LA MASCOTA ------------------//
	HWND grpDatosMascota = CreateWindowEx(0, "BUTTON", "Datos de la mascota",
		BS_GROUPBOX | WS_VISIBLE | WS_CHILD | WS_GROUP,
		10, 345, 565, 125, hWnd, (HMENU)GRP_DATOSMASCOTA, hInst, NULL);
	wndprocGrpMascota = (WNDPROC)SetWindowLongPtr(grpDatosMascota, GWL_WNDPROC, (LONG_PTR)procGrpMascota);

	CreateWindowEx(0, "STATIC", "Nombre:  ",
		WS_VISIBLE | WS_CHILD,
		10, 30, 100, 20, grpDatosMascota, NULL, hInst, NULL);

	HWND hwndNombreMascota = CreateWindowEx(0, "EDIT", "",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_UPPERCASE,
		115, 30, 150, 20, grpDatosMascota, (HMENU)TXT_NOMBREMASCOTA, hInst, NULL);
	wndprocRegCitaNombreMascota = (WNDPROC)SetWindowLongPtr(hwndNombreMascota, GWL_WNDPROC, (LONG_PTR)procRegCitaNombreMascota);

	CreateWindowEx(0, "STATIC", "Especie:  ",
		WS_VISIBLE | WS_CHILD,
		10, 60, 100, 20, grpDatosMascota, NULL, hInst, NULL);

	HWND hwndCbEspecie = CreateWindowEx(0, "COMBOBOX", "",
		WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | WS_VSCROLL | CBS_SIMPLE,
		115, 60, 150, 120, grpDatosMascota, (HMENU)CB_ESPECIEMASCOTA, hInst, NULL);

	SendMessage(hwndCbEspecie, CB_ADDSTRING, 0, (LPARAM)"");
	SendMessage(hwndCbEspecie, CB_ADDSTRING, 0, (LPARAM)"PERRO");
	SendMessage(hwndCbEspecie, CB_ADDSTRING, 0, (LPARAM)"GATO");
	SendMessage(hwndCbEspecie, CB_ADDSTRING, 0, (LPARAM)"ERIZO");
	SendMessage(hwndCbEspecie, CB_ADDSTRING, 0, (LPARAM)"HAMSTER");
	SendMessage(hwndCbEspecie, CB_ADDSTRING, 0, (LPARAM)"CUYO");
	SendMessage(hwndCbEspecie, CB_ADDSTRING, 0, (LPARAM)"AVE");
	SendMessage(hwndCbEspecie, CB_ADDSTRING, 0, (LPARAM)"REPTIL");

	CreateWindowEx(0, "BUTTON", "Macho",
		BS_AUTORADIOBUTTON | WS_VISIBLE | WS_CHILD,
		10, 90, 100, 20, grpDatosMascota, (HMENU)RB_MACHOMASCOTA, hInst, NULL);

	CreateWindowEx(0, "BUTTON", "Hembra",
		BS_AUTORADIOBUTTON | WS_VISIBLE | WS_CHILD,
		115, 90, 100, 20, grpDatosMascota, (HMENU)RB_HEMBRAMASCOTA, hInst, NULL);

	CheckRadioButton(grpDatosMascota, RB_MACHOMASCOTA, RB_HEMBRAMASCOTA, RB_MACHOMASCOTA);


	// --------- Fuera de todos los groupbox ----------------//

	CreateWindowEx(0, "BUTTON", "Registrar cita",
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_BORDER,
		10, 480, 130, 30, hWnd, (HMENU)BTN_REGISTRARCITA, hInst, NULL);

	CreateWindowEx(0, "BUTTON", "Actualizar cita",
		WS_CHILD | BS_PUSHBUTTON | WS_BORDER,
		10, 480, 130, 30, hWnd, (HMENU)BTN_ACTUALIZARCITA, hInst, NULL);

	CreateWindowEx(0, "BUTTON", "Nueva",
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_BORDER,
		150, 480, 100, 30, hWnd, (HMENU)BTN_LIMPIARREGISTRARCITA, hInst, NULL);

	SetFocus(hwndFechaCita);
}

void InsertarControlesAgenda(HWND hWnd) {

	//-------------- Insertar menu ---------------//
	HMENU hmCitas, hmContenedor = GetMenu(hWnd);
	hmContenedor = CreateMenu();
	hmCitas = CreateMenu();

	AppendMenu(hmCitas, MF_STRING, MENU_CITAS_NUEVA, "Nueva cita");

	AppendMenu(hmContenedor, MF_STRING | MF_POPUP, NULL, "");
	AppendMenu(hmContenedor, MF_STRING | MF_POPUP, MENU_PERFIL, doctor.NombreCompleto().c_str());
	AppendMenu(hmContenedor, MF_STRING | MF_POPUP, (UINT)hmCitas, "Citas");
	AppendMenu(hmContenedor, MF_STRING | MF_POPUP, MENU_SALIR, "Salir");

	HBITMAP bitdoctor = (HBITMAP)LoadImage(NULL, doctor.rutaFoto.c_str(), IMAGE_BITMAP, 25, 25, LR_LOADFROMFILE);
	HBITMAP bitimage = (HBITMAP)LoadImage(NULL, "page.bmp", IMAGE_BITMAP, 15, 15, LR_LOADFROMFILE);
	SetMenuItemBitmaps(hmCitas, 0, MF_BYPOSITION, bitimage, bitimage);
	ModifyMenu(hmContenedor, 0, MF_BYPOSITION | MF_BITMAP | MF_STRING, 0, (LPCTSTR)bitdoctor);
	SetMenu(hWnd, hmContenedor);
	//----------------------------------------------------//
	CreateWindowEx(0, "STATIC", "Filtro por fecha: ",
		WS_CHILD | WS_VISIBLE,
		20, 20, 120, 20, hWnd, (HMENU)LBL_AGENDA_FILTRO, hInst, NULL);

	HWND hwndCbxFiltro = CreateWindowEx(0, "COMBOBOX", "",
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | WS_VSCROLL|CBS_SIMPLE|WS_BORDER,
		130, 18, 130, 150, hWnd, (HMENU)CBX_FILTRARPORFECHA, hInst, NULL);
	
	aux = origen;
	SendMessage(hwndCbxFiltro, CB_ADDSTRING, 0, (LPARAM)"");
	for (int i = 0; i < citas; i++) {
		if (aux != origen) {
			if (!(aux->datosCita.fechaCita.fechaCompleta == aux->ant->datosCita.fechaCita.fechaCompleta)) {
				SendMessage(hwndCbxFiltro, CB_ADDSTRING, 0, (LPARAM)aux->datosCita.fechaCita.fechaCompleta.c_str());
			}
		}
		else {
			SendMessage(hwndCbxFiltro, CB_ADDSTRING, 0, (LPARAM)aux->datosCita.fechaCita.fechaCompleta.c_str());
		}
		aux = aux->sig;
	}

	HWND hwndBtnQuitarFiltro = CreateWindowEx(0, "BUTTON", "",
		WS_CHILD | BS_BITMAP | WS_BORDER|BS_FLAT,
		265, 20, 20, 20, hWnd, (HMENU)BTN_QUITARFILTRO, hInst, NULL);
	HBITMAP hbQuitarFiltro = (HBITMAP)LoadImage(NULL, "cancel.bmp", IMAGE_BITMAP, 15, 15, LR_LOADFROMFILE);
	SendMessage(hwndBtnQuitarFiltro,BM_SETIMAGE,IMAGE_BITMAP,(LPARAM)hbQuitarFiltro);

	CreateWindowEx(0, "STATIC", "Citas próximas",
		WS_CHILD | WS_VISIBLE,
		20, 50, 100, 20, hWnd, (HMENU)LBL_AGENDA_AGENDA, hInst, NULL);

	CreateWindowEx(0, "STATIC", "",
		WS_CHILD | WS_VISIBLE,
		560, 50, 80, 20, hWnd, (HMENU)LBL_PAGINA_AGENDA, hInst, NULL);

	InsertarAgenda(SIMPLE,1,"", hWnd);
}

void MostrarVentanaPrincipal() {
	//Creamos nuestra ventana y la mostramos
	hwndWndAgenda = CreateWindowEx(0, "wcWndAgenda", "Agenda de veterinaria",
		WS_SYSMENU,
		0, 0, 675, 500, NULL, 0, hInst, NULL);
	SetWindowPos(hwndWndAgenda, HWND_TOPMOST, CenterToScreenX(hwndWndAgenda), CenterToScreenY(hwndWndAgenda), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	ShowWindow(hwndWndAgenda, SW_SHOW);
}

bool RegistrarClases() {
	WNDCLASSEX wc;

	//Llenamos los datos para nuestra ventana principal de login
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = (WNDPROC)procLogin;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "wcWndLogin";
	if (!RegisterClassEx(&wc)) return false; //Registramos nuestra venta, sino es posible salimos de la app

	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInst;
	wc.lpfnWndProc = (WNDPROC)procPerfil;
	wc.lpszClassName = "wcWndPerfil";
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = CreateSolidBrush(RGB(180, 180, 180));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.lpszMenuName = NULL;
	if (!RegisterClassEx(&wc)) return false; //Registramos nuestra venta, sino es posible salimos de la app

	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInst;
	wc.lpfnWndProc = (WNDPROC)procCitaNueva;
	wc.lpszClassName = "wcWndCitaNueva";
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = CreateSolidBrush(RGB(180, 180, 180));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.lpszMenuName = NULL;
	if (!RegisterClassEx(&wc)) return false; //Registramos nuestra venta, sino es posible salimos de la app

	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInst;
	wc.lpfnWndProc = (WNDPROC)procConfig;
	wc.lpszClassName = "wcWndConfig";
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.lpszMenuName = NULL;
	if (!RegisterClassEx(&wc)) return false; //Registramos nuestra venta, sino es posible salimos de la app

	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInst;
	wc.lpfnWndProc = (WNDPROC)procAgenda;
	wc.lpszClassName = "wcWndAgenda";
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = CreateSolidBrush(RGB(139, 188, 255));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(hInst,IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.lpszMenuName = NULL;
	if (!RegisterClassEx(&wc)) return false; //Registramos nuestra venta, sino es posible salimos de la app

	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInst;
	wc.lpfnWndProc = (WNDPROC)procResumenCita;
	wc.lpszClassName = "wcWndResumenCita";
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = CreateSolidBrush(RGB(180,180,180));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.lpszMenuName = NULL;
	if (!RegisterClassEx(&wc)) return false; //Registramos nuestra venta, sino es posible salimos de la app

	return true;
}

int CenterToScreenX(HWND ventana) {
	RECT rc;
	GetWindowRect(ventana, &rc);
	return ((GetSystemMetrics(SM_CXSCREEN) - rc.right) / 2);
}

int CenterToScreenY(HWND ventana) {
	RECT rc;
	GetWindowRect(ventana, &rc);
	return ((GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2);
}

bool ValidarLogin(HWND hWnd) {
	//Variables
	char vUsuario[20] = "", vContra[25] = "";
	HWND hwndTxtUsuario = GetDlgItem(hwndWndLogin, TXT_USUARIO);
	HWND hwndTxtContra = GetDlgItem(hwndWndLogin, TXT_PASS);

	//Guardamos el contenido de los textbox
	SendMessage(hwndTxtUsuario, WM_GETTEXT, 20, (LPARAM)vUsuario);
	SendMessage(hwndTxtContra, WM_GETTEXT, 25, (LPARAM)vContra);

	//Validamos que no estén vacíos los campos
	if (strcmp(vUsuario, "") == 0 || strcmp(vContra, "") == 0) {
		Mensajes::OperacionNoValida("No puede haber campos vacíos");
		SetFocus(hwndTxtUsuario); //Ponemos el foco en el campo 'usuario'
		return false;
	}

	//Validamos que sea el usuario correcto
	if (strcmp(vUsuario, "alex") == 0 && strcmp(vContra, "123") == 0) return true;
	else {
		Mensajes::OperacionNoValida("Datos incorrectos");

		//Limpiamos los campos y ponemos el foco en el campo 'usuario'
		SetWindowText(hwndTxtUsuario, "");
		SetWindowText(hwndTxtContra, "");
		SetFocus(hwndTxtUsuario);
	}
	return false;
}

LRESULT CALLBACK procLogin(HWND hWnd, UINT mensaje, WPARAM wParam, LPARAM lParam) {
	switch (mensaje)
	{
	case WM_CREATE:
		InsertarControlesLogin(hWnd); //Llamamos a la macro para insertar los controles en la ventana
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case BTN_ENTRAR:
			if (HIWORD(wParam) == BN_CLICKED) {
				if (ValidarLogin(hWnd)) {
					ShowWindow(hwndWndLogin, SW_HIDE); //Ocultamos la ventana de login
					MostrarVentanaPrincipal();
				}
			}
			break;
		default:
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, mensaje, wParam, lParam);
		break;
	}

	return 0;
}

LRESULT CALLBACK procPerfil(HWND hWnd, UINT mensaje, WPARAM wParam, LPARAM lParam) {
	//static bool edicionActivada;
	switch (mensaje)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case BTN_ELEGIRFOTODOCTOR:
		{
			OPENFILENAME ofn;
			char ruta[MAX_PATH] = { 0 };
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFile = ruta;
			ofn.nMaxFile = sizeof(ruta);
			ofn.lpstrFilter = ("Imagenes BMP (*.bmp)");
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetOpenFileName(&ofn)) {
				doctor.rutaFotoTemp = ruta;
				SendMessage(hWnd, WM_PAINT, wParam, lParam);
			}

		}
		break;
		case MENUPERFIL_EDITAR:
		{
			EnableWindow(GetDlgItem(hWnd, TXT_CEDULADOCTOR), true);
			EnableWindow(GetDlgItem(hWnd, TXT_NOMBREDOCTOR), true);
			EnableWindow(GetDlgItem(hWnd, TXT_APELLIDODOCTOR), true);
			EnableWindow(GetDlgItem(hWnd, TXT_TELEFONODOCTOR), true);
			EnableWindow(GetDlgItem(hWnd, TXT_CORREODOCTOR), true);

			ShowWindow(GetDlgItem(hWnd, BTN_ELEGIRFOTODOCTOR), SW_SHOW);

			AppendMenu(GetMenu(hWnd), MF_STRING | MF_POPUP, MENUPERFIL_CANCELAR, "Cancelar");
			ModifyMenu(GetMenu(hWnd), 0, MF_BYPOSITION, MENUPERFIL_GUARDAR, "Guardar");
			DrawMenuBar(hWnd);
		}
		break;
		case MENUPERFIL_GUARDAR:
		{
			char cedula[10], nombre[52], apellido[72], telefono[12], correo[52];
			if(doctor.rutaFotoTemp != "")
				doctor.rutaFoto = doctor.rutaFotoTemp;
			GetWindowText(GetDlgItem(hWnd, TXT_CEDULADOCTOR), cedula, 10);
			GetWindowText(GetDlgItem(hWnd, TXT_NOMBREDOCTOR), nombre, 52);
			GetWindowText(GetDlgItem(hWnd, TXT_APELLIDODOCTOR), apellido, 72);
			GetWindowText(GetDlgItem(hWnd, TXT_TELEFONODOCTOR), telefono, 12);
			GetWindowText(GetDlgItem(hWnd, TXT_CORREODOCTOR), correo, 52);

			//Guardamos en la estructura
			doctor.cedula = cedula;
			doctor.nombre = nombre;
			doctor.apellido = apellido;
			doctor.telefono = telefono;
			doctor.correo = correo;

			ModifyMenu(GetMenu(hwndWndAgenda), 1, MF_BYPOSITION, MENU_PERFIL, doctor.NombreCompleto().c_str());
			DrawMenuBar(hwndWndAgenda);

			HBITMAP bitdoctor = (HBITMAP)LoadImage(NULL, doctor.rutaFoto.c_str(), IMAGE_BITMAP, 25, 25, LR_LOADFROMFILE);
			ModifyMenu(GetMenu(hwndWndAgenda), 0, MF_BYPOSITION | MF_BITMAP | MF_POPUP, 0, (LPCTSTR)bitdoctor);
			DrawMenuBar(hwndWndAgenda);

			DestroyWindow(hWnd);
			Mensajes::OperacionExitosa("¡Datos actualizados!");
		}
		break;
		case MENUPERFIL_CANCELAR:
			//SetFocus(hWnd);
			EnableWindow(GetDlgItem(hWnd, TXT_CEDULADOCTOR), false);
			EnableWindow(GetDlgItem(hWnd, TXT_NOMBREDOCTOR), false);
			EnableWindow(GetDlgItem(hWnd, TXT_APELLIDODOCTOR), false);
			EnableWindow(GetDlgItem(hWnd, TXT_TELEFONODOCTOR), false);
			EnableWindow(GetDlgItem(hWnd, TXT_CORREODOCTOR), false);

			SetWindowText(GetDlgItem(hWnd, TXT_CEDULADOCTOR), doctor.cedula.c_str());
			SetWindowText(GetDlgItem(hWnd, TXT_NOMBREDOCTOR), doctor.nombre.c_str());
			SetWindowText(GetDlgItem(hWnd, TXT_APELLIDODOCTOR), doctor.apellido.c_str());
			SetWindowText(GetDlgItem(hWnd, TXT_TELEFONODOCTOR), doctor.telefono.c_str());
			SetWindowText(GetDlgItem(hWnd, TXT_CORREODOCTOR), doctor.correo.c_str());

			ShowWindow(GetDlgItem(hWnd, BTN_ELEGIRFOTODOCTOR), SW_HIDE);

			RemoveMenu(GetMenu(hWnd), 1, MF_BYPOSITION);
			ModifyMenu(GetMenu(hWnd), 0, MF_BYPOSITION, MENUPERFIL_EDITAR, "Editar perfil");
			DrawMenuBar(hWnd);
			break;
		}
		break;
	case WM_PAINT:
	{
		HDC dcMem, dcCliente = NULL;
		PAINTSTRUCT ps;
		HBITMAP hbmFotoDoctor = (HBITMAP)LoadImage(NULL, doctor.rutaFoto.c_str(), IMAGE_BITMAP, 150, 150, LR_LOADFROMFILE);
		dcCliente = BeginPaint(hWnd, &ps);
		dcMem = CreateCompatibleDC(dcCliente);
		BITMAP bmFotoDoctor;
		SelectObject(dcMem, hbmFotoDoctor);
		GetObject(hbmFotoDoctor, sizeof(BITMAP), &bmFotoDoctor);
		BitBlt(dcCliente, 340, 20, bmFotoDoctor.bmWidth, bmFotoDoctor.bmHeight, dcMem, 0, 0, SRCCOPY);
		DeleteDC(dcMem);
		EndPaint(hWnd, &ps);
		DeleteObject(hbmFotoDoctor);
	}
	return DefWindowProc(hWnd, mensaje, wParam, lParam);
	case WM_CTLCOLORSTATIC:
	{
		HDC hdcStatic = (HDC)wParam;

		if ((HWND)lParam == GetDlgItem(hWnd, TXT_CEDULADOCTOR)) break;
		if ((HWND)lParam == GetDlgItem(hWnd, TXT_NOMBREDOCTOR)) break;
		if ((HWND)lParam == GetDlgItem(hWnd, TXT_APELLIDODOCTOR)) break;
		if ((HWND)lParam == GetDlgItem(hWnd, TXT_TELEFONODOCTOR)) break;
		if ((HWND)lParam == GetDlgItem(hWnd, TXT_CORREODOCTOR)) break;

		if ((HWND)lParam == GetDlgItem(hWnd, LBL_INFODOCTOR_ERROR)) {
			HBRUSH hbrushStaticBackground = CreateSolidBrush(RGB(255, 0, 0));
			SetBkColor(hdcStatic, RGB(255, 0, 0));
			return (LRESULT)hbrushStaticBackground;
		}

		HBRUSH hbrushStaticBackground = CreateSolidBrush(RGB(180, 180, 180));

		SetBkColor(hdcStatic, RGB(180, 180, 180));
		return (LRESULT)hbrushStaticBackground;
	}
	break;
	case WM_CREATE:
	{
		InsertarControlesPerfil(hWnd);

		SetWindowText(GetDlgItem(hWnd, TXT_CEDULADOCTOR), doctor.cedula.c_str());
		SetWindowText(GetDlgItem(hWnd, TXT_NOMBREDOCTOR), doctor.nombre.c_str());
		SetWindowText(GetDlgItem(hWnd, TXT_APELLIDODOCTOR), doctor.apellido.c_str());
		SetWindowText(GetDlgItem(hWnd, TXT_TELEFONODOCTOR), doctor.telefono.c_str());
		SetWindowText(GetDlgItem(hWnd, TXT_CORREODOCTOR), doctor.correo.c_str());
	}
	break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, mensaje, wParam, lParam);
		break;
	}
	return 0;
}

LRESULT CALLBACK procCitaNueva(HWND hWnd, UINT mensaje, WPARAM wParam, LPARAM lParam) {
	switch (mensaje)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case BTN_LIMPIARREGISTRARCITA:
			ShowWindow(GetDlgItem(hWnd, BTN_ACTUALIZARCITA), SW_HIDE);
			ShowWindow(GetDlgItem(hWnd, BTN_REGISTRARCITA), SW_SHOW);
			LimpiarCamposRegistroCita();
			break;
		case BTN_REGISTRARCITA:
			AgregarCita();
			break;
		case BTN_ACTUALIZARCITA:
			ActualizarCita();
			break;
		default:
			break;
		}
		break;
	case WM_CTLCOLORSTATIC:
	{
		//if ((HWND)lParam == GetDlgItem(hWnd, TXT_NUMEROCITA)) break;
		//if ((HWND)lParam == GetDlgItem(hWnd, TXT_COSTOCITA)) break;

		HBRUSH hbrStatics = CreateSolidBrush(RGB(180, 180, 180));
		SetBkColor((HDC)wParam, RGB(180, 180, 180));
		return (LRESULT)hbrStatics;
	}
	break;
	case WM_CREATE:
		InsertarControlesCitaNueva(hWnd);
		//SetFocus(GetDlgItem(GetDlgItem(hWnd,GRP_DATOSCITA),DT_FECHACITA));
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, mensaje, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK procConfig(HWND hWnd, UINT mensaje, WPARAM wParam, LPARAM lParam) {
	switch (mensaje)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, mensaje, wParam, lParam);
	}
}

LRESULT CALLBACK procAgenda(HWND hWnd, UINT mensaje, WPARAM wParam, LPARAM lParam) {
	/*static HDC hdcCliente, hdcTemp;
	static PAINTSTRUCT ps;
	static HBITMAP hbmGatito , hbmPerrito;*/
	static HDC dcMem = NULL, dcClient = NULL;
	static PAINTSTRUCT ps;
	switch (mensaje)
	{
	case WM_PAINT:
	{
		//Ponemos el perro en el fondo
		
		
		HBITMAP hbmImgPerro = (HBITMAP)LoadImage(NULL, "perro.bmp", IMAGE_BITMAP, 220, 220, LR_LOADFROMFILE);
		dcClient = BeginPaint(hWnd, &ps);
		dcMem = CreateCompatibleDC(dcClient);
		BITMAP bmImgPerro;
		SelectObject(dcMem, hbmImgPerro);
		GetObject(hbmImgPerro, sizeof(BITMAP), &bmImgPerro);
		BitBlt(dcClient, 450, 250, bmImgPerro.bmWidth, bmImgPerro.bmHeight, dcMem, 0, 0, SRCCOPY);
		DeleteDC(dcMem);
		EndPaint(hWnd, &ps);
		DeleteObject(hbmImgPerro);
		//Ponemos el perro en el fondo
		/*
		hdcTemp = CreateCompatibleDC(hdcCliente);
		BITMAP bmGatito, bmPerrito;

		hbmGatito = LoadBitmap(hInst,MAKEINTRESOURCE(IMG_GATITO));
		hbmPerrito = LoadBitmap(hInst, MAKEINTRESOURCE(IMG_PERRITO));

		SelectObject(hdcTemp, hbmGatito);
		GetObject(hbmGatito, sizeof(BITMAP), &bmGatito);
		StretchBlt(hdcCliente, ps.rcPaint.left, ps.rcPaint.bottom-bmGatito.bmHeight+100, bmGatito.bmWidth-100, bmGatito.bmHeight-100, hdcTemp, 0, 0,bmGatito.bmWidth,bmGatito.bmHeight,SRCCOPY);

		SelectObject(hdcTemp, hbmPerrito);
		GetObject(hbmPerrito, sizeof(BITMAP), &bmPerrito);
		BitBlt(hdcCliente, ps.rcPaint.right - bmPerrito.bmWidth, ps.rcPaint.bottom - bmPerrito.bmHeight, bmPerrito.bmWidth, bmPerrito.bmHeight, hdcTemp, 0, 0, SRCCOPY);

		DeleteObject(hbmGatito);
		DeleteObject(hbmPerrito);
		DeleteDC(hdcTemp);
		EndPaint(hWnd, &ps);*/

		//return DefWindowProc(hWnd, mensaje, wParam, lParam);
	}
	break;
	case WM_CTLCOLORSTATIC:
	{
		//if (LOWORD(wParam) == GRID) break;
		HDC hdcStaticAgenda = (HDC)wParam;
		if ((HWND)lParam == GetDlgItem(hWnd, LBL_AGENDA_FILTRO)) {
			HBRUSH hbrushStaticAgenda = CreateSolidBrush(RGB(139, 188, 255));
			SetBkColor(hdcStaticAgenda, RGB(139, 188, 255));
			return (LRESULT)hbrushStaticAgenda;
		}
		if ((HWND)lParam == GetDlgItem(hWnd, LBL_AGENDA_AGENDA)) {
			HBRUSH hbrushStaticAgenda = CreateSolidBrush(RGB(139, 188, 255));
			SetBkColor(hdcStaticAgenda, RGB(139, 188, 255));
			return (LRESULT)hbrushStaticAgenda;
		}
		if ((HWND)lParam == GetDlgItem(hWnd, LBL_PAGINA_AGENDA)) {
			HBRUSH hbrushStaticAgenda = CreateSolidBrush(RGB(139, 188, 255));
			SetBkColor(hdcStaticAgenda, RGB(139, 188, 255));
			return (LRESULT)hbrushStaticAgenda;
		}
		if ((HWND)lParam == GetDlgItem(hWnd, LBL_AGENDA_ID)) {
			HBRUSH hbrushStaticAgenda = CreateSolidBrush(RGB(185, 185, 185));
			SetBkColor(hdcStaticAgenda, RGB(185, 185, 185));
			return (LRESULT)hbrushStaticAgenda;
		}
		if ((HWND)lParam == GetDlgItem(hWnd, LBL_AGENDA_FECHA)) {
			HBRUSH hbrushStaticAgenda = CreateSolidBrush(RGB(185, 185, 185));
			SetBkColor(hdcStaticAgenda, RGB(185, 185, 185));
			return (LRESULT)hbrushStaticAgenda;
		}
		if ((HWND)lParam == GetDlgItem(hWnd, LBL_AGENDA_HORA)) {
			HBRUSH hbrushStaticAgenda = CreateSolidBrush(RGB(185, 185, 185));
			SetBkColor(hdcStaticAgenda, RGB(185, 185, 185));
			return (LRESULT)hbrushStaticAgenda;
		}
		if ((HWND)lParam == GetDlgItem(hWnd, LBL_AGENDA_NOMBRECLIENTE)) {
			HBRUSH hbrushStaticAgenda = CreateSolidBrush(RGB(185, 185, 185));
			SetBkColor(hdcStaticAgenda, RGB(185, 185, 185));
			return (LRESULT)hbrushStaticAgenda;
		}
		if ((HWND)lParam == GetDlgItem(hWnd, LBL_AGENDA_ACCIONES)) {
			HBRUSH hbrushStaticAgenda = CreateSolidBrush(RGB(185, 185, 185));
			SetBkColor(hdcStaticAgenda, RGB(185, 185, 185));
			return (LRESULT)hbrushStaticAgenda;
		}
		HBRUSH hbrushStaticAgenda = CreateSolidBrush(RGB(255, 255, 255));
		SetBkColor(hdcStaticAgenda, RGB(255, 255, 255));
		return (LRESULT)hbrushStaticAgenda;
	}
		break;
	case WM_CREATE:
	{
		//dcClient = BeginPaint(hWnd, &ps);
		/*HICON iconoPrograma = LoadIcon(GetModuleHandle(NULL), "edit.bmp");
		SendMessage(hWnd,WM_SETICON,ICON_BIG,(LPARAM)iconoPrograma);*/
		CargarDatosCita();
		CargarDatosDoctor();
		InsertarControlesAgenda(hWnd);
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case BTN_QUITARFILTRO:
			SendMessage(GetDlgItem(hWnd, CBX_FILTRARPORFECHA), CB_SETCURSEL, 0, 0);
			ShowWindow(GetDlgItem(hWnd, BTN_QUITARFILTRO), SW_HIDE);
			ShowWindow(GetDlgItem(hWnd, LBL_PAGINA_AGENDA), SW_SHOW);
			InsertarAgenda(EDITAR, 1, "", hWnd);
			break;
		case CBX_FILTRARPORFECHA:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				char cFecha[20];
				GetWindowText(GetDlgItem(hWnd, CBX_FILTRARPORFECHA), cFecha, 20);
				string strFecha = cFecha;

				if (strFecha != "") {
					InsertarAgenda(EDITAR, 1, strFecha, hWnd);
					ShowWindow(GetDlgItem(hWnd, BTN_QUITARFILTRO), SW_SHOW);
				}
			}
			
			break;
		case BTN_VERINFOCITA:
		{
			int nIdCita;
			char idCita[5];
			HWND botonPresionado = GetFocus();
			GetWindowText(GetNextDlgTabItem(hWnd, GetNextDlgTabItem(hWnd, GetNextDlgTabItem(hWnd, GetNextDlgTabItem(hWnd, botonPresionado, true), true), true), true), idCita, 5);
			nIdCita = atoi(idCita);
			Cita *citaBuscada = BuscarCitaPorId(nIdCita);

			//if (IsWindowVisible(hwndWndResumenCita)) break;
			hwndWndResumenCita = CreateWindowEx(0, "wcWndResumenCita", "Cita", WS_POPUP|WS_BORDER,
				0, 0, 348, 402, NULL, NULL, hInst, NULL);
			SetWindowPos(hwndWndResumenCita, 0, CenterToScreenX(hwndWndResumenCita), CenterToScreenY(hwndWndResumenCita), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			ShowWindow(hwndWndResumenCita, SW_SHOW);

			SetWindowText(GetDlgItem(hwndWndResumenCita, LBL_RESUMEN_NUMCITA), to_string(citaBuscada->datosCita.NumCita).c_str());
			SetWindowText(GetDlgItem(hwndWndResumenCita, LBL_RESUMEN_FECHA), citaBuscada->datosCita.fechaCita.fechaCompleta.c_str());
			SetWindowText(GetDlgItem(hwndWndResumenCita, LBL_RESUMEN_HORA), citaBuscada->datosCita.horaCita.horaCompleta.c_str());
			SetWindowText(GetDlgItem(hwndWndResumenCita, LBL_RESUMEN_NOMBRECLIENTE), citaBuscada->datosCliente.nombreCompleto().c_str());
			SetWindowText(GetDlgItem(hwndWndResumenCita, LBL_RESUMEN_TELEFONO), citaBuscada->datosCliente.telefono.c_str());
			SetWindowText(GetDlgItem(hwndWndResumenCita, LBL_RESUMEN_NOMBREMASCOTA), citaBuscada->datosMascota.nombre.c_str());
			SetWindowText(GetDlgItem(hwndWndResumenCita, LBL_RESUMEN_ESPECIE), citaBuscada->datosMascota.especie.c_str());
			SetWindowText(GetDlgItem(hwndWndResumenCita, LBL_RESUMEN_MOTIVO), citaBuscada->datosCita.motivoConsulta.c_str());
			string texto = "$" + to_string(citaBuscada->datosCita.costo);
			SetWindowText(GetDlgItem(hwndWndResumenCita, LBL_RESUMEN_COSTO), texto.c_str());

		}
			break;
		case BTN_EDITARCITA:
		{
			char idCita[5];
			HWND botonPresionado = GetFocus();
			GetWindowText(GetNextDlgTabItem(hWnd,GetNextDlgTabItem(hWnd,GetNextDlgTabItem(hWnd,GetNextDlgTabItem(hWnd,GetNextDlgTabItem(hWnd, botonPresionado, true),true),true),true),true),idCita,5);
			int nIdCita = atoi(idCita);
			Cita *citaBuscada = BuscarCitaPorId(nIdCita);

			if (!IsWindowVisible(hwndWndCitaNueva)) {
				hwndWndCitaNueva = CreateWindowEx(0, "wcWndCitaNueva", "Agendar nueva cita",
					WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT,
					600, 585, NULL, NULL, hInst, NULL);
				SetWindowPos(hwndWndCitaNueva, 0, CenterToScreenX(hwndWndCitaNueva), CenterToScreenY(hwndWndCitaNueva), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
				ShowWindow(hwndWndCitaNueva, SW_SHOW);
			}

			//Llenamos todo el formulario con la información de la cita
			HWND hwndGrpDatosCita = GetDlgItem(hwndWndCitaNueva, GRP_DATOSCITA);
			HWND hwndGrpDatosCliente = GetDlgItem(hwndWndCitaNueva, GRP_DATOSCLIENTE);
			HWND hwndGrpDatosMascota = GetDlgItem(hwndWndCitaNueva, GRP_DATOSMASCOTA);
			SetWindowText(GetDlgItem(hwndGrpDatosCita, TXT_NUMEROCITA), to_string(citaBuscada->datosCita.NumCita).c_str());
			SetWindowText(GetDlgItem(hwndGrpDatosCita, TXT_COSTOCITA), to_string(citaBuscada->datosCita.costo).c_str());
			SetWindowText(GetDlgItem(hwndGrpDatosCita, TXT_MOTIVOCONSULTA), citaBuscada->datosCita.motivoConsulta.c_str());
			//SendMessage(GetDlgItem(hwndGrpDatosCita, DT_FECHACITA), DateTime_SetSystemtime, 0, 0);
			//DateTime_SetSystemtime()
			SYSTEMTIME lpSysTime;
			
			//ZeroMemory(&lpSysTime, sizeof(LPSYSTEMTIME));
			GetSystemTime(&lpSysTime);
			lpSysTime.wYear = citaBuscada->datosCita.fechaCita.ano;
			lpSysTime.wMonth = citaBuscada->datosCita.fechaCita.mes;
			lpSysTime.wDay = citaBuscada->datosCita.fechaCita.dia;
			lpSysTime.wHour = citaBuscada->datosCita.horaCita.hora;
			lpSysTime.wMinute = citaBuscada->datosCita.horaCita.minuto;
			DateTime_SetSystemtime(GetDlgItem(hwndGrpDatosCita, DT_FECHACITA), GDT_VALID, &lpSysTime);
			DateTime_SetSystemtime(GetDlgItem(hwndGrpDatosCita, DT_HORACITA), GDT_VALID, &lpSysTime);
			EnableWindow(GetDlgItem(hwndGrpDatosCita, DT_FECHACITA), false);
			EnableWindow(GetDlgItem(hwndGrpDatosCita, DT_HORACITA), false);

			SetWindowText(GetDlgItem(hwndGrpDatosCliente, TXT_NOMBRECLIENTE), citaBuscada->datosCliente.nombre.c_str());
			SetWindowText(GetDlgItem(hwndGrpDatosCliente, TXT_APELLIDOCLIENTE), citaBuscada->datosCliente.apellido.c_str());
			SetWindowText(GetDlgItem(hwndGrpDatosCliente, TXT_TELEFONOCLIENTE), citaBuscada->datosCliente.telefono.c_str());

			SetWindowText(GetDlgItem(hwndGrpDatosMascota, TXT_NOMBREMASCOTA), citaBuscada->datosMascota.nombre.c_str());
			SendMessage(GetDlgItem(hwndGrpDatosMascota, CB_ESPECIEMASCOTA), CB_SELECTSTRING, 0, (LPARAM)citaBuscada->datosMascota.especie.c_str());
			if (citaBuscada->datosMascota.esMacho == true)
				CheckRadioButton(hwndGrpDatosMascota, RB_MACHOMASCOTA, RB_HEMBRAMASCOTA, RB_MACHOMASCOTA);
			else
				CheckRadioButton(hwndGrpDatosMascota, RB_MACHOMASCOTA, RB_HEMBRAMASCOTA, RB_HEMBRAMASCOTA);
			
			ShowWindow(GetDlgItem(hwndWndCitaNueva, BTN_REGISTRARCITA), SW_HIDE);
			ShowWindow(GetDlgItem(hwndWndCitaNueva, BTN_ACTUALIZARCITA), SW_SHOW);

		}
			break;
		case BTN_CANCELARCITA:
		{
			char idCita[5], nombre[70];
			HWND botonPresionado = GetFocus();
			GetWindowText(GetNextDlgTabItem(hWnd, GetNextDlgTabItem(hWnd, GetNextDlgTabItem(hWnd, GetNextDlgTabItem(hWnd, GetNextDlgTabItem(hWnd, GetNextDlgTabItem(hWnd, botonPresionado, true), true), true), true), true), true), idCita, 5);
			//GetWindowText(GetNextDlgTabItem(hWnd, GetNextDlgTabItem(hWnd, GetNextDlgTabItem(hWnd, GetNextDlgTabItem(hWnd, GetNextDlgTabItem(hWnd, botonPresionado, true), true), true), true), true), idCita, 5);
			int nIdCita = atoi(idCita);
			GetWindowText(GetNextDlgTabItem(hWnd, GetNextDlgTabItem(hWnd, GetNextDlgTabItem(hWnd,botonPresionado, true), true), true), nombre, 70);
			string mensaje = "¿Realmente deseas eliminar permanentemente la cita de ";
			mensaje.append(nombre);
			mensaje.append("?");
			int respuesta = Mensajes::Confirmacion(mensaje);
			if (respuesta == IDYES) {
				
				Cita *citaBuscada = BuscarCitaPorId(nIdCita);

				//Esto se ejecuta si es el único registro
				if (citaBuscada == origen && citaBuscada == ultimo) {
					delete citaBuscada;
					aux = origen = ultimo = NULL;
				}else if (citaBuscada == origen) {
					origen->sig->ant = NULL;
					origen = origen->sig;
					delete citaBuscada;
				}
				else if (citaBuscada == ultimo) {
					ultimo->ant->sig = NULL;
					ultimo = ultimo->ant;
					delete citaBuscada;
				}
				else {
					citaBuscada->ant->sig = citaBuscada->sig;
					citaBuscada->sig->ant = citaBuscada->ant;
					delete citaBuscada;
				}
				citas--;
				InsertarAgenda(ELIMINAR,1,"", hWnd);
			}
		}
			break;
		case BTN_PAGINACION:
		{
			HWND botonPresionado = GetFocus();
			char texto[2];
			GetWindowText(botonPresionado, texto, 2);
			byte pag = atoi(texto);
			InsertarAgenda(EDITAR,pag,"",hWnd);
		}
			break;
		case MENU_PERFIL:
			if (IsWindowVisible(hwndWndPerfil)) break;
			hwndWndPerfil = CreateWindowEx(0, "wcWndPerfil", "Perfil",
				WS_SYSMENU,
				0, 0, 521, 300, NULL, NULL, hInst, NULL);
			SetWindowPos(hwndWndPerfil, 0, CenterToScreenX(hwndWndPerfil), CenterToScreenY(hwndWndPerfil), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			ShowWindow(hwndWndPerfil, SW_SHOW);
			break;
		case MENU_CITAS_NUEVA:
			if (IsWindowVisible(hwndWndCitaNueva)) break;
			hwndWndCitaNueva = CreateWindowEx(0, "wcWndCitaNueva", "Agendar nueva cita",
				WS_SYSMENU, 0, 0,
				600, 585, NULL, NULL, hInst, NULL);
			SetWindowPos(hwndWndCitaNueva, 0, CenterToScreenX(hwndWndCitaNueva), CenterToScreenY(hwndWndCitaNueva), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			ShowWindow(hwndWndCitaNueva, SW_SHOW);
			break;
		case MENU_SALIR:
			ConfirmarSalir();
			break;
		}
		break;
	case WM_CLOSE:
	{
		ConfirmarSalir();
	}
	break;
	case WM_DESTROY:
		//Destruimos los objetos globales
		//DeleteObject(hbmFotoDoctor);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, mensaje, wParam, lParam);
		break;
	}
}

LRESULT CALLBACK procTxtContra(HWND hWnd, UINT mensaje, WPARAM wParam, LPARAM lParam) {
	switch (mensaje)
	{
	case WM_GETDLGCODE:
		if (LOWORD(wParam) == VK_RETURN) {
			if (ValidarLogin(hwndWndLogin)) {
				ShowWindow(hwndWndLogin, SW_HIDE);
				MostrarVentanaPrincipal();
			}
		}
		else {
			return CallWindowProc(wndprocTxtContra, hWnd, mensaje, wParam, lParam);
		}
		break;
	default:
		return CallWindowProc(wndprocTxtContra, hWnd, mensaje, wParam, lParam);
		break;
	}
	return 0;
}

LRESULT CALLBACK procTxtCedula(HWND hWnd, UINT mensaje, WPARAM wParam, LPARAM lParam) {
	switch (mensaje)
	{
	case WM_KEYDOWN:
	{
		char cedula[20];
		byte tamanoCedula = 0;

		GetWindowText(GetDlgItem(hwndWndPerfil, TXT_CEDULADOCTOR), cedula, 20);
		string strCedula(cedula);
		//Validamos que solo inserte números
		if (!EsNumero(strCedula)) {
			SetWindowText(GetDlgItem(hwndWndPerfil, LBL_INFODOCTOR_ERROR), "La cédula solo puede contener números");
			ShowWindow(GetDlgItem(hwndWndPerfil, LBL_INFODOCTOR_ERROR), SW_SHOWNOACTIVATE);
			break;
		}
		else {
			SetWindowText(GetDlgItem(hwndWndPerfil, LBL_INFODOCTOR_ERROR), "");
			ShowWindow(GetDlgItem(hwndWndPerfil, LBL_INFODOCTOR_ERROR), SW_HIDE);
		}

		//Validamos que no sobrepasé el límite
		tamanoCedula = strlen(cedula);
		if (LOWORD(wParam) == VK_BACK)
		{
			if (tamanoCedula > 0)
				tamanoCedula--;
		}
		else
			tamanoCedula++;
		if (tamanoCedula > 8) {
			SetWindowText(GetDlgItem(hwndWndPerfil, LBL_INFODOCTOR_ERROR), "La cédula no puede sobrepasar los 8 dígitos");
			ShowWindow(GetDlgItem(hwndWndPerfil, LBL_INFODOCTOR_ERROR), SW_SHOWNOACTIVATE);
		}
		else {
			SetWindowText(GetDlgItem(hwndWndPerfil, LBL_INFODOCTOR_ERROR), "");
			ShowWindow(GetDlgItem(hwndWndPerfil, LBL_INFODOCTOR_ERROR), SW_HIDE);
		}
	}
	break;
	default:
		return CallWindowProc(wndprocTxtCedula, hWnd, mensaje, wParam, lParam);
		break;
	}
}

LRESULT CALLBACK procTxtNombre(HWND hWnd, UINT mensaje, WPARAM wParam, LPARAM lParam) {
	switch (mensaje)
	{
	case WM_KEYDOWN:
	{
		char nombre[50];
		GetWindowText(GetDlgItem(hwndWndPerfil, TXT_NOMBREDOCTOR), nombre, 50);
		string strNombre(nombre);

		if (!EsTexto(strNombre)) {
			SetWindowText(GetDlgItem(hwndWndPerfil, LBL_INFODOCTOR_ERROR), "El nombre solo puede contener letras");
			ShowWindow(GetDlgItem(hwndWndPerfil, LBL_INFODOCTOR_ERROR), SW_SHOWNOACTIVATE);
			break;
		}
		else {
			SetWindowText(GetDlgItem(hwndWndPerfil, LBL_INFODOCTOR_ERROR), "");
			ShowWindow(GetDlgItem(hwndWndPerfil, LBL_INFODOCTOR_ERROR), SW_HIDE);
		}
	}
	break;
	default:
		return CallWindowProc(wndprocTxtNombre, hWnd, mensaje, wParam, lParam);
		break;
	}
}

LRESULT CALLBACK procTxtApellido(HWND hWnd, UINT mensaje, WPARAM wParam, LPARAM lParam) {
	switch (mensaje)
	{
	case WM_KEYDOWN:
	{
		char apellido[70];
		GetWindowText(GetDlgItem(hwndWndPerfil, TXT_APELLIDODOCTOR), apellido, 70);
		string strApellido(apellido);

		if (!EsTexto(strApellido)) {
			SetWindowText(GetDlgItem(hwndWndPerfil, LBL_INFODOCTOR_ERROR), "El apellido solo puede contener letras");
			ShowWindow(GetDlgItem(hwndWndPerfil, LBL_INFODOCTOR_ERROR), SW_SHOWNOACTIVATE);
			break;
		}
		else {
			SetWindowText(GetDlgItem(hwndWndPerfil, LBL_INFODOCTOR_ERROR), "");
			ShowWindow(GetDlgItem(hwndWndPerfil, LBL_INFODOCTOR_ERROR), SW_HIDE);
		}
	}
	break;
	default:
		return CallWindowProc(wndprocTxtApellido, hWnd, mensaje, wParam, lParam);
		break;
	}
}

LRESULT CALLBACK procTxtTelefono(HWND hWnd, UINT mensaje, WPARAM wParam, LPARAM lParam) {
	switch (mensaje)
	{
	case WM_KEYDOWN:
	{
		char telefono[20];
		byte tamanoTelefono;
		GetWindowText(GetDlgItem(hwndWndPerfil, TXT_TELEFONODOCTOR), telefono, 20);
		string strTelefono(telefono);

		//Validamos que solo sean números
		if (!EsNumero(strTelefono)) {
			SetWindowText(GetDlgItem(hwndWndPerfil, LBL_INFODOCTOR_ERROR), "El teléfono solo puede contener números");
			ShowWindow(GetDlgItem(hwndWndPerfil, LBL_INFODOCTOR_ERROR), SW_SHOWNOACTIVATE);
			break;
		}
		else {
			SetWindowText(GetDlgItem(hwndWndPerfil, LBL_INFODOCTOR_ERROR), "");
			ShowWindow(GetDlgItem(hwndWndPerfil, LBL_INFODOCTOR_ERROR), SW_HIDE);
		}

		//Validamos que solo pueda tener como máximo 10 digitos
		tamanoTelefono = strlen(telefono);
		if (LOWORD(wParam) == VK_BACK)
		{
			if (tamanoTelefono > 0)
				tamanoTelefono--;
		}
		else
		{
			switch (LOWORD(wParam))
			{
			case 0x30:case 0x60:
			case 0x31:case 0x61:
			case 0x32:case 0x62:
			case 0x33:case 0x63:
			case 0x34:case 0x64:
			case 0x35:case 0x65:
			case 0x36:case 0x66:
			case 0x37:case 0x67:
			case 0x38:case 0x68:
			case 0x39:case 0x69:
				tamanoTelefono++;
			}
		}

		if (tamanoTelefono > 10) {
			SetWindowText(GetDlgItem(hwndWndPerfil, LBL_INFODOCTOR_ERROR), "El teléfono no puede sobrepasar los 10 dígitos");
			ShowWindow(GetDlgItem(hwndWndPerfil, LBL_INFODOCTOR_ERROR), SW_SHOWNOACTIVATE);
		}
		else {
			SetWindowText(GetDlgItem(hwndWndPerfil, LBL_INFODOCTOR_ERROR), "");
			ShowWindow(GetDlgItem(hwndWndPerfil, LBL_INFODOCTOR_ERROR), SW_HIDE);
		}
	}
	break;
	default:
		return CallWindowProc(wndprocTxtApellido, hWnd, mensaje, wParam, lParam);
	}
}

bool EsNumero(string miCadena) {
	for (int i = 0; i < miCadena.length(); i++) {
		if (!(miCadena[i] >= '0' && miCadena[i] <= '9')) return false;
	}
	return true;
}

bool EsTexto(string miCadena) {
	int i;
	bool resultado = false;
	for (i = 0; i < miCadena.length(); i++) {
		if ((miCadena[i] >= 'A' && miCadena[i] <= 'Z') || (miCadena[i] >= 'a' && miCadena[i] <= 'z') || miCadena[i]==8 || miCadena[i]==32)
			resultado = true;
		else {
			resultado = false;
			break;
		}
	}
	return resultado;
}

void DesglozarFecha(string cadena, byte *dia, byte *mes, unsigned int *ano) {
	*dia = (byte)atoi(cadena.substr(0, 2).c_str());
	*mes = (byte)atoi(cadena.substr(3, 2).c_str());
	*ano = atoi(cadena.substr(6, 4).c_str());
}

void DesglozarTiempo(string cadena, byte *hora, byte *minuto) {
	*hora = (byte)atoi(cadena.substr(0, 2).c_str());
	*minuto = (byte)atoi(cadena.substr(3, 2).c_str());
}

void AgregarCita() {
	if (!ValidarCita()) return; //Antes que nada validamos que toda la información esté validada

	//Variables
	string sTexto;
	char textoCorto[255], textoLargo[1024];

	//Variables de datos de la cita
	unsigned int numCita;
	byte dia, mes, hora, minuto;
	unsigned int ano;
	float costo;
	string strFecha, strHora, motivo;

	//Variables de datos del cliente
	string nombreCliente, apellidoCliente, telefonoCliente;
	//Variables de datos de la mascota
	string nombreMascota, especieMascota;
	bool esMacho;

	HWND hwndGrpDatosCita = GetDlgItem(hwndWndCitaNueva, GRP_DATOSCITA);
	HWND hwndGrpDatosCliente = GetDlgItem(hwndWndCitaNueva, GRP_DATOSCLIENTE);
	HWND hwndGrpDatosMascota = GetDlgItem(hwndWndCitaNueva, GRP_DATOSMASCOTA);

	GetWindowText(GetDlgItem(hwndGrpDatosCita, TXT_NUMEROCITA), textoCorto, 255);
	numCita = atoi(textoCorto);
	GetWindowText(GetDlgItem(hwndGrpDatosCita, DT_FECHACITA), textoCorto, 255);
	strFecha = textoCorto;
	sTexto = textoCorto;
	DesglozarFecha(sTexto, &dia, &mes, &ano);
	GetWindowText(GetDlgItem(hwndGrpDatosCita, DT_HORACITA), textoCorto, 255);
	strHora = textoCorto;
	sTexto = textoCorto;
	DesglozarTiempo(sTexto, &hora, &minuto);
	GetWindowText(GetDlgItem(hwndGrpDatosCita, TXT_COSTOCITA), textoCorto, 255);
	costo = stof(textoCorto);
	GetWindowText(GetDlgItem(hwndGrpDatosCita, TXT_MOTIVOCONSULTA), textoCorto, 255);
	motivo = textoCorto;

	GetWindowText(GetDlgItem(hwndGrpDatosCliente, TXT_NOMBRECLIENTE), textoCorto, 255);
	nombreCliente = textoCorto;
	GetWindowText(GetDlgItem(hwndGrpDatosCliente, TXT_APELLIDOCLIENTE), textoCorto, 255);
	apellidoCliente = textoCorto;
	GetWindowText(GetDlgItem(hwndGrpDatosCliente, TXT_TELEFONOCLIENTE), textoCorto, 255);
	telefonoCliente = textoCorto;

	GetWindowText(GetDlgItem(hwndGrpDatosMascota, TXT_NOMBREMASCOTA), textoCorto, 255);
	nombreMascota = textoCorto;
	GetWindowText(GetDlgItem(hwndGrpDatosMascota, CB_ESPECIEMASCOTA), textoCorto, 255);
	especieMascota = textoCorto;
	if (IsDlgButtonChecked(hwndGrpDatosMascota, RB_MACHOMASCOTA) == BST_CHECKED)
		esMacho = true;
	else
		esMacho = false;

	aux = origen;

	if (origen == NULL) {
		aux = new Cita;
		aux->ant = NULL;
		aux->sig = NULL;
		origen = aux;
		ultimo = origen;

		//Llenamos los datos
		aux->datosCita.NumCita = 1;
		aux->datosCita.fechaCita.dia = dia;
		aux->datosCita.fechaCita.mes = mes;
		aux->datosCita.fechaCita.ano = ano;
		aux->datosCita.fechaCita.fechaCompleta = strFecha;
		aux->datosCita.horaCita.hora = hora;
		aux->datosCita.horaCita.minuto = minuto;
		aux->datosCita.horaCita.horaCompleta = strHora;
		aux->datosCita.costo = costo;
		aux->datosCita.motivoConsulta = motivo;
		aux->datosCliente.nombre = nombreCliente;
		aux->datosCliente.apellido = apellidoCliente;
		aux->datosCliente.telefono = telefonoCliente;
		aux->datosMascota.nombre = nombreMascota;
		aux->datosMascota.especie = especieMascota;
		aux->datosMascota.esMacho = esMacho;

	}
	else {
		Fecha fechaCita;
		fechaCita.dia = dia;
		fechaCita.mes = mes;
		fechaCita.ano = ano;

		Hora horaCita;
		horaCita.hora = hora;
		horaCita.minuto = minuto;
		
		int resCompFechaOrigen = CompararFecha(fechaCita, origen->datosCita.fechaCita);
		int resCompHoraOrigen = CompararHora(horaCita, origen->datosCita.horaCita);
		int resCompFechaUltimo = CompararFecha(fechaCita, ultimo->datosCita.fechaCita);
		int resCompHoraUltimo = CompararHora(horaCita, ultimo->datosCita.horaCita);
		int resCompCitaOrigen=0, resCompCitaUltimo=0;

		if (resCompFechaOrigen == -1) resCompCitaOrigen = -1;
		else if (resCompFechaOrigen == 0 && resCompHoraOrigen == -1) resCompCitaOrigen = -1;
		else if (resCompFechaOrigen == 0 && resCompHoraOrigen == 0) resCompCitaOrigen = 0;
		else resCompCitaOrigen = 1;

		if (resCompFechaUltimo == -1) resCompCitaUltimo = -1;
		else if (resCompFechaUltimo == 0 && resCompHoraUltimo == -1) resCompCitaUltimo = -1;
		else if (resCompFechaUltimo == 0 && resCompHoraUltimo == 0) resCompCitaUltimo = 0;
		else resCompCitaUltimo = 1;

		//Verificamos si la cita es menor que la primera cita
		if (resCompCitaOrigen == -1) {
			origen->ant = new Cita;
			aux = origen->ant;
			aux->ant = NULL;
			aux->sig = origen;
			origen = aux;
		}//Sino verificamos si la cita es igual a la primera
		else if(resCompCitaOrigen == 0){
			Mensajes::OperacionNoValida("No puede haber 2 citas en la misma fecha y hora");
			aux = origen;
			return;
		}//Sino verificamos si la cita es mayor a la ultima
		else if (resCompCitaUltimo == 1) {
			ultimo->sig = new Cita;
			aux = ultimo->sig;
			aux->ant = ultimo;
			aux->sig = NULL;
			ultimo = aux;
		}//Sino verificamos si la cita es igual a la ultima
		else if (resCompCitaUltimo == 0) {
			Mensajes::OperacionNoValida("No puede haber 2 citas en la misma fecha y hora");
			aux = origen;
			return;
		}
		else {
			bool resultado = false;
			int min=0, max = citas, mitad=0;

			do {
				mitad = (min + max) / 2;
				aux = origen;
				//Avanzamos a la mitad de nuestra lista
				for (int i = 0; i < mitad; i++) {
					aux = aux->sig;
				}

				int resCompFechaAux = CompararFecha(fechaCita, aux->datosCita.fechaCita);
				int resCompHoraAux = CompararHora(horaCita, aux->datosCita.horaCita);
				int resCompCitaAux=0;

				if (resCompFechaAux == -1) resCompCitaAux = -1;
				else if (resCompFechaAux == 0 && resCompHoraAux == -1) resCompCitaAux = -1;
				else if (resCompFechaAux == 0 && resCompHoraAux == 0) resCompCitaAux = 0;
				else resCompCitaAux = 1;

				if (resCompCitaAux == 0) {
					Mensajes::OperacionNoValida("No puede haber 2 citas en la misma fecha y hora");
					aux = origen;
					return;
				}
				else {

					if (aux==origen) {
						aux->sig->ant = new Cita;
						aux->sig->ant->sig = aux->sig;
						aux->sig = aux->sig->ant;
						aux->sig->ant = aux;
						aux = aux->sig;
						resultado = true;
					}
					else if (aux==ultimo) {
						aux->ant->sig = new Cita;
						aux->ant->sig->ant = aux->ant;
						aux->ant = aux->ant->sig;
						aux->ant->sig = aux;
						aux = aux->ant;
						resultado = true;
					}
					else {
						int resCompFechaAnt = -2, resCompHoraAnt = -2, resCompFechaSig = -2, resCompHoraSig = -2;
						int resCompCitaAnt = 0, resCompCitaSig = 0;

						resCompFechaAnt = CompararFecha(fechaCita, aux->ant->datosCita.fechaCita);
						resCompHoraAnt = CompararHora(horaCita, aux->ant->datosCita.horaCita);
						resCompFechaSig = CompararFecha(fechaCita, aux->sig->datosCita.fechaCita);
						resCompHoraSig = CompararHora(horaCita, aux->sig->datosCita.horaCita);

						if (resCompFechaAnt == -1) resCompCitaAnt = -1;
						else if (resCompFechaAnt == 0 && resCompHoraAnt == -1) resCompCitaAnt = -1;
						else if (resCompFechaAnt == 0 && resCompHoraAnt == 0) resCompCitaAnt = 0;
						else resCompCitaAnt = 1;

						if (resCompFechaSig == -1) resCompCitaSig = -1;
						else if (resCompFechaSig == 0 && resCompHoraSig == -1) resCompCitaSig = -1;
						else if (resCompFechaSig == 0 && resCompHoraSig == 0) resCompCitaSig = 0;
						else resCompCitaSig = 1;

						if (resCompCitaAux == -1 && resCompCitaAnt == 1) {
							aux->ant->sig = new Cita;
							aux->ant->sig->ant = aux->ant;
							aux->ant->sig->sig = aux;
							aux->ant = aux->ant->sig;
							aux = aux->ant;
							resultado = true;
						}//Sino verificamos si está a la derecha de aux
						else if (resCompCitaAux == 1 && resCompCitaSig == -1) {
							aux->sig->ant = new Cita;
							aux->sig->ant->sig = aux->sig;
							aux->sig->ant->ant = aux;
							aux->sig = aux->sig->ant;
							aux = aux->sig;
							resultado = true;
						}//Sino 
						else {
							//Entonces vemos si la cita es menor aux
							if (resCompCitaAux == -1) {
								max = mitad;
							}//Sino entonces la cita es mayor
							else {
								min = mitad;
							}
						}
					}
				}

			} while (resultado == false);

		}



		//Llenamos los datos si no es el origen xD
		aux->datosCita.NumCita = numCita;
		aux->datosCita.fechaCita.dia = dia;
		aux->datosCita.fechaCita.mes = mes;
		aux->datosCita.fechaCita.ano = ano;
		aux->datosCita.fechaCita.fechaCompleta = strFecha;
		aux->datosCita.horaCita.hora = hora;
		aux->datosCita.horaCita.minuto = minuto;
		aux->datosCita.horaCita.horaCompleta = strHora;
		aux->datosCita.costo = costo;
		aux->datosCita.motivoConsulta = motivo;
		aux->datosCliente.nombre = nombreCliente;
		aux->datosCliente.apellido = apellidoCliente;
		aux->datosCliente.telefono = telefonoCliente;
		aux->datosMascota.nombre = nombreMascota;
		aux->datosMascota.especie = especieMascota;
		aux->datosMascota.esMacho = esMacho;
	}

	citas++;
	LimpiarCamposRegistroCita();
	SetWindowText(GetDlgItem(GetDlgItem(hwndWndCitaNueva, GRP_DATOSCITA), TXT_NUMEROCITA), TraerNumNuevaCita().c_str());
	
	//ListView_DeleteAllItems(GetDlgItem(hwndWndAgenda, LV_AGENDA)); //Limpiamos el listview
	//LlenarAgenda(GetDlgItem(hwndWndAgenda, LV_AGENDA));
	InsertarAgenda(AGREGAR,1,"", hwndWndAgenda);
	//SendMessage(hwndWndAgenda, WM_CTLCOLORSTATIC, 0, 0);
	Mensajes::OperacionExitosa("Se ha agregado la cita a su agenda correctamente");
}

void GuardarTodo() {
	fstream archivo;
	string dato;

	aux = origen;
	if (origen != NULL) {
		archivo.open("DatosCitas.txt", ios::trunc | ios::binary | ios::out);
		if (archivo.is_open()) {
			while (aux != NULL) {
				if (aux == origen) dato = to_string(aux->datosCita.NumCita) + "%%";
				else dato = "\n" + to_string(aux->datosCita.NumCita) + "%%";

				archivo.write(dato.c_str(), dato.length());

				dato = aux->datosCita.fechaCita.fechaCompleta + "%%";
				archivo.write(dato.c_str(), dato.length());

				dato = aux->datosCita.horaCita.horaCompleta + "%%";
				archivo.write(dato.c_str(), dato.length());

				dato = to_string(aux->datosCita.costo) + "%%";
				archivo.write(dato.c_str(), dato.length());

				dato = aux->datosCita.motivoConsulta + "%%";
				archivo.write(dato.c_str(), dato.length());

				dato = aux->datosCliente.nombre + "%%";
				archivo.write(dato.c_str(), dato.length());

				dato = aux->datosCliente.apellido + "%%";
				archivo.write(dato.c_str(), dato.length());

				dato = aux->datosCliente.telefono + "%%";
				archivo.write(dato.c_str(), dato.length());

				dato = aux->datosMascota.nombre + "%%";
				archivo.write(dato.c_str(), dato.length());

				dato = aux->datosMascota.especie + "%%";
				archivo.write(dato.c_str(), dato.length());

				dato = aux->datosMascota.esMacho == true ? "true" : "false";
				archivo.write(dato.c_str(), dato.length());


				aux = aux->sig;
			}
			archivo.close();
		}
		else {
			Mensajes::Error();
		}
	}
	else {
		archivo.open("DatosCitas.txt", ios::trunc | ios::binary | ios::out);
		archivo.close();
		remove("DatosCitas.txt");
	}

	//Ahora guardamos los datos del doctor
	archivo.open("DatosDoctor.txt", ios::trunc | ios::binary | ios::out);
	if (archivo.is_open()) {
		doctor.cedula.append("\n");
		archivo.write(doctor.cedula.c_str(), doctor.cedula.length());

		doctor.nombre.append("\n");
		archivo.write(doctor.nombre.c_str(), doctor.nombre.length());

		doctor.apellido.append("\n");
		archivo.write(doctor.apellido.c_str(), doctor.apellido.length());

		doctor.telefono.append("\n");
		archivo.write(doctor.telefono.c_str(), doctor.telefono.length());

		doctor.correo.append("\n");
		archivo.write(doctor.correo.c_str(), doctor.correo.length());

		archivo.write(doctor.rutaFoto.c_str(), doctor.rutaFoto.length());

		archivo.close();
	}
	else {
		Mensajes::Error();
	}

}

void CargarDatosCita() {
	fstream archivo;
	int inicio, fin;
	string dato, linea;

	aux = origen = ultimo = NULL;

	archivo.open("DatosCitas.txt");
	if (archivo.is_open()) {

		while (archivo.good()) {
			getline(archivo, linea);
			if (origen == NULL) {
				aux = new Cita;
				aux->sig = NULL;
				aux->ant = NULL;
				ultimo = origen = aux;

				//Buscamos el primer dato
				inicio = 0;
				fin = linea.find("%%");
				dato = linea.substr(inicio, fin - inicio);
				aux->datosCita.NumCita = atoi(dato.c_str());

				inicio = fin + 2;
				fin = linea.find("%%", inicio);
				dato = linea.substr(inicio, fin - inicio);
				aux->datosCita.fechaCita.fechaCompleta = dato;
				DesglozarFecha(dato, &aux->datosCita.fechaCita.dia, &aux->datosCita.fechaCita.mes, &aux->datosCita.fechaCita.ano);

				inicio = fin + 2;
				fin = linea.find("%%", inicio);
				dato = linea.substr(inicio, fin - inicio);
				aux->datosCita.horaCita.horaCompleta = dato;
				DesglozarTiempo(dato, &aux->datosCita.horaCita.hora, &aux->datosCita.horaCita.minuto);

				inicio = fin + 2;
				fin = linea.find("%%", inicio);
				dato = linea.substr(inicio, fin - inicio);
				aux->datosCita.costo = atof(dato.c_str());

				inicio = fin + 2;
				fin = linea.find("%%", inicio);
				dato = linea.substr(inicio, fin - inicio);
				aux->datosCita.motivoConsulta = dato;

				inicio = fin + 2;
				fin = linea.find("%%", inicio);
				dato = linea.substr(inicio, fin - inicio);
				aux->datosCliente.nombre = dato;

				inicio = fin + 2;
				fin = linea.find("%%", inicio);
				dato = linea.substr(inicio, fin - inicio);
				aux->datosCliente.apellido = dato;

				inicio = fin + 2;
				fin = linea.find("%%", inicio);
				dato = linea.substr(inicio, fin - inicio);
				aux->datosCliente.telefono = dato;

				inicio = fin + 2;
				fin = linea.find("%%", inicio);
				dato = linea.substr(inicio, fin - inicio);
				aux->datosMascota.nombre = dato;

				inicio = fin + 2;
				fin = linea.find("%%", inicio);
				dato = linea.substr(inicio, fin - inicio);
				aux->datosMascota.especie = dato;

				inicio = fin + 2;
				fin = linea.find("%%", inicio);
				dato = linea.substr(inicio, fin - inicio);

				aux->datosMascota.esMacho = dato == "true" ? true : false;

			}
			else {
				ultimo->sig = new Cita;
				aux = ultimo->sig;
				aux->ant = ultimo;
				aux->sig = NULL;
				ultimo = aux;

				//Buscamos el primer dato
				inicio = 0;
				fin = linea.find("%%");
				dato = linea.substr(inicio, fin - inicio);
				aux->datosCita.NumCita = atoi(dato.c_str());

				inicio = fin + 2;
				fin = linea.find("%%", inicio);
				dato = linea.substr(inicio, fin - inicio);
				aux->datosCita.fechaCita.fechaCompleta = dato;
				DesglozarFecha(dato, &aux->datosCita.fechaCita.dia, &aux->datosCita.fechaCita.mes, &aux->datosCita.fechaCita.ano);

				inicio = fin + 2;
				fin = linea.find("%%", inicio);
				dato = linea.substr(inicio, fin - inicio);
				aux->datosCita.horaCita.horaCompleta = dato;
				DesglozarTiempo(dato, &aux->datosCita.horaCita.hora, &aux->datosCita.horaCita.minuto);

				inicio = fin + 2;
				fin = linea.find("%%", inicio);
				dato = linea.substr(inicio, fin - inicio);
				aux->datosCita.costo = atof(dato.c_str());

				inicio = fin + 2;
				fin = linea.find("%%", inicio);
				dato = linea.substr(inicio, fin - inicio);
				aux->datosCita.motivoConsulta = dato;

				inicio = fin + 2;
				fin = linea.find("%%", inicio);
				dato = linea.substr(inicio, fin - inicio);
				aux->datosCliente.nombre = dato;

				inicio = fin + 2;
				fin = linea.find("%%", inicio);
				dato = linea.substr(inicio, fin - inicio);
				aux->datosCliente.apellido = dato;

				inicio = fin + 2;
				fin = linea.find("%%", inicio);
				dato = linea.substr(inicio, fin - inicio);
				aux->datosCliente.telefono = dato;

				inicio = fin + 2;
				fin = linea.find("%%", inicio);
				dato = linea.substr(inicio, fin - inicio);
				aux->datosMascota.nombre = dato;

				inicio = fin + 2;
				fin = linea.find("%%", inicio);
				dato = linea.substr(inicio, fin - inicio);
				aux->datosMascota.especie = dato;

				inicio = fin + 2;
				fin = linea.find("%%", inicio);
				dato = linea.substr(inicio, fin - inicio);

				aux->datosMascota.esMacho = dato == "true" ? true : false;
			}

			citas++;
		}
		archivo.close();
	}

}

bool ValidarCita() {
	//Declaración de variables
	char textoCorto[255], textoLargo[1024];

	byte dia, mes;
	byte hora, minuto;
	unsigned int ano;

	string strNumCita, strFechaCita, strHoraCita, strCostoCita, strMotivoCita;
	string strNombreCliente, strApellidoCliente, strTelefonoCliente;
	string strNombreMascota, strEspecieMascota;

	HWND hwndGrpDatosCita = GetDlgItem(hwndWndCitaNueva, GRP_DATOSCITA);
	HWND hwndGrpDatosCliente = GetDlgItem(hwndWndCitaNueva, GRP_DATOSCLIENTE);
	HWND hwndGrpDatosMascota = GetDlgItem(hwndWndCitaNueva, GRP_DATOSMASCOTA);

	//Obtenemos los datos del formulario
	{
		GetWindowText(GetDlgItem(hwndGrpDatosCita, TXT_NUMEROCITA), textoCorto, 255);
		strNumCita = textoCorto;
		GetWindowText(GetDlgItem(hwndGrpDatosCita, DT_FECHACITA), textoCorto, 255);
		strFechaCita = textoCorto;
		DesglozarFecha(strFechaCita, &dia, &mes, &ano);
		GetWindowText(GetDlgItem(hwndGrpDatosCita, DT_HORACITA), textoCorto, 255);
		strHoraCita = textoCorto;
		DesglozarTiempo(strHoraCita, &hora, &minuto);
		GetWindowText(GetDlgItem(hwndGrpDatosCita, TXT_COSTOCITA), textoCorto, 255);
		strCostoCita = textoCorto;
		GetWindowText(GetDlgItem(hwndGrpDatosCita, TXT_MOTIVOCONSULTA), textoLargo, 1024);
		strMotivoCita = textoLargo;

		GetWindowText(GetDlgItem(hwndGrpDatosCliente, TXT_NOMBRECLIENTE), textoCorto, 255);
		strNombreCliente = textoCorto;
		GetWindowText(GetDlgItem(hwndGrpDatosCliente, TXT_APELLIDOCLIENTE), textoCorto, 255);
		strApellidoCliente = textoCorto;
		GetWindowText(GetDlgItem(hwndGrpDatosCliente, TXT_TELEFONOCLIENTE), textoCorto, 255);
		strTelefonoCliente = textoCorto;

		GetWindowText(GetDlgItem(hwndGrpDatosMascota, TXT_NOMBREMASCOTA), textoCorto, 255);
		strNombreMascota = textoCorto;
		GetWindowText(GetDlgItem(hwndGrpDatosMascota, CB_ESPECIEMASCOTA), textoCorto, 255);
		strEspecieMascota = textoCorto;
	}

	//Validamos que no haya datos vacios
	{
		if (strCostoCita == "") {
			Mensajes::OperacionNoValida("El campo 'Costo' se encuentra vacío");
			SetFocus(GetDlgItem(hwndGrpDatosCita, TXT_COSTOCITA));
			return false;
		}
		if (strMotivoCita == "") {
			Mensajes::OperacionNoValida("El campo 'Motivo de la consulta' se encuentra vacío");
			SetFocus(GetDlgItem(hwndGrpDatosCita, TXT_MOTIVOCONSULTA));
			return false;
		}

		if (strNombreCliente == "") {
			Mensajes::OperacionNoValida("El campo 'Nombre del cliente' se encuentra vacío");
			SetFocus(GetDlgItem(hwndGrpDatosCliente, TXT_NOMBRECLIENTE));
			return false;
		}
		if (strApellidoCliente == "") {
			Mensajes::OperacionNoValida("El campo 'Apellido del cliente' se encuentra vacío");
			SetFocus(GetDlgItem(hwndGrpDatosCliente, TXT_APELLIDOCLIENTE));
			return false;
		}
		if (strTelefonoCliente == "") {
			Mensajes::OperacionNoValida("El campo 'Teléfono' se encuentra vacío");
			SetFocus(GetDlgItem(hwndGrpDatosCliente, TXT_TELEFONOCLIENTE));
			return false;
		}

		if (strNombreMascota == "") {
			Mensajes::OperacionNoValida("El campo 'Nombre de la mascota' se encuentra vacío");
			SetFocus(GetDlgItem(hwndGrpDatosMascota, TXT_NOMBREMASCOTA));
			return false;
		}
		if (strEspecieMascota == "") {
			Mensajes::OperacionNoValida("El campo 'Especie de la mascota' se encuentra vacío");
			SetFocus(GetDlgItem(hwndGrpDatosMascota, CB_ESPECIEMASCOTA));
			return false;
		}
	}

	//--------------- Hacemos otro tipo de validaciones ---------------------//
	//Validamos que no haya puesto una fecha pasada

	//Validamos que el costo de la cita sea correcto

	Fecha fechaCita, fechaHoy;
	fechaCita.dia = dia;
	fechaCita.mes = mes;
	fechaCita.ano = ano;

	Hora horaCita, horaActual;
	horaCita.hora = hora;
	horaCita.minuto = minuto;

	time_t tiempo = time(0);
	tm *tiempoDesglozado = localtime(&tiempo);

	fechaHoy.dia = tiempoDesglozado->tm_mday;
	fechaHoy.mes = tiempoDesglozado->tm_mon + 1;
	fechaHoy.ano = tiempoDesglozado->tm_year + 1900;

	//char nHoraActual[40];
	//strftime(nHoraActual, 40, "%I:%M:%S", tiempoDesglozado);

	horaActual.hora = tiempoDesglozado->tm_hour;
	horaActual.minuto = tiempoDesglozado->tm_min;
	/*if (horaActual.hora > 12) horaActual.periodo = 'p';
	else horaActual.periodo = 'a';*/

	int resultadoFecha = CompararFecha(fechaCita, fechaHoy);

	if (resultadoFecha == -1)
	{
		Mensajes::OperacionNoValida("Ingrese una fecha de cita válida");
		SetFocus(GetDlgItem(hwndGrpDatosCita, DT_FECHACITA));
		return false;
	}
	else if (resultadoFecha == 0) {
		if (CompararHora(horaCita, horaActual) == -1 || CompararHora(horaCita, horaActual) == 0)
		{
			Mensajes::OperacionNoValida("Ingrese una hora de cita válida");
			SetFocus(GetDlgItem(hwndGrpDatosCita, DT_HORACITA));
			return false;
		}
	}

	if (!EsMoneda(strCostoCita))
	{
		Mensajes::OperacionNoValida("Inrese un monto correcto (Ej. 000.00)");
		SetFocus(GetDlgItem(hwndGrpDatosCita, TXT_COSTOCITA));
		return false;
	}

	if (!EsTexto(strNombreCliente)) {
		Mensajes::OperacionNoValida("El nombre del cliente solo puede contener texto");
		SetFocus(GetDlgItem(hwndGrpDatosCliente, TXT_NOMBRECLIENTE));
		return false;
	}

	if (!EsTexto(strApellidoCliente)) {
		Mensajes::OperacionNoValida("El apellido del cliente solo puede contener texto");
		SetFocus(GetDlgItem(hwndGrpDatosCliente, TXT_APELLIDOCLIENTE));
		return false;
	}

	if (!EsTexto(strNombreMascota)) {
		Mensajes::OperacionNoValida("El nombre de la mascota solo puede contener texto");
		SetFocus(GetDlgItem(hwndGrpDatosMascota, TXT_NOMBREMASCOTA));
		return false;
	}

	if (!(strTelefonoCliente.length() == 8 || strTelefonoCliente.length() == 10 || strTelefonoCliente.length() == 12)) {
		Mensajes::OperacionNoValida("El número de teléfono solo puede contener 8, 10 u 12 caracteres");
		SetFocus(GetDlgItem(hwndGrpDatosCliente, TXT_TELEFONOCLIENTE));
		return false;
	}

	if (!EsNumero(strTelefonoCliente)) {
		Mensajes::OperacionNoValida("El número de teléfono solo debe contener números");
		SetFocus(GetDlgItem(hwndGrpDatosCliente, TXT_TELEFONOCLIENTE));
		return false;
	}

	return true;
}

void LimpiarCamposRegistroCita() {
	HWND hwndGrpDatosCita = GetDlgItem(hwndWndCitaNueva, GRP_DATOSCITA);
	HWND hwndGrpDatosCliente = GetDlgItem(hwndWndCitaNueva, GRP_DATOSCLIENTE);
	HWND hwndGrpDatosMascota = GetDlgItem(hwndWndCitaNueva, GRP_DATOSMASCOTA);

	SetWindowText(GetDlgItem(hwndGrpDatosCita, TXT_NUMEROCITA), "");
	SetWindowText(GetDlgItem(hwndGrpDatosCita, DT_HORACITA), "10:00:00 p.m.");
	SetWindowText(GetDlgItem(hwndGrpDatosCita, TXT_COSTOCITA), "");
	SetWindowText(GetDlgItem(hwndGrpDatosCita, TXT_MOTIVOCONSULTA), "");

	SetWindowText(GetDlgItem(hwndGrpDatosCliente, TXT_NOMBRECLIENTE), "");
	SetWindowText(GetDlgItem(hwndGrpDatosCliente, TXT_APELLIDOCLIENTE), "");
	SetWindowText(GetDlgItem(hwndGrpDatosCliente, TXT_TELEFONOCLIENTE), "");

	SetWindowText(GetDlgItem(hwndGrpDatosMascota, TXT_NOMBREMASCOTA), "");
	SendMessage(GetDlgItem(hwndGrpDatosMascota, CB_ESPECIEMASCOTA), CB_SETCURSEL, 0, 0);

	SetWindowText(GetDlgItem(hwndGrpDatosCita, TXT_NUMEROCITA), TraerNumNuevaCita().c_str());
	CheckRadioButton(hwndGrpDatosMascota, RB_MACHOMASCOTA, RB_HEMBRAMASCOTA, RB_MACHOMASCOTA);
}

void ConfirmarSalir() {
	//int respuesta = MessageBox(hwndWndPrincipal, "¿Realmente deseas salir de la aplicación?", "Confirmación", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION);
	int respuesta = Mensajes::Confirmacion("¿Realmente deseas salir de la aplicación?");
	if (respuesta == IDYES) {
		GuardarTodo();
		DestroyWindow(hwndWndAgenda);
	}
}

string TraerNumNuevaCita() {
	aux = origen;

	if (origen == NULL) return "1";

	int mayor=aux->datosCita.NumCita;
	while (aux != NULL) {
		if (aux->datosCita.NumCita > mayor)
			mayor = aux->datosCita.NumCita;
		aux = aux->sig;
	}

	return to_string(mayor + 1);
}

bool EstaVacio(string cadena) {
	return cadena == "" ? true : false;
}

bool EsMoneda(string cadena) {
	byte punto = 0;
	for (int i = 0; i < cadena.length(); i++) {
		if (cadena[i] == '.') punto++;
		if (punto > 1) return false;
		if (!((cadena[i] >= '0' && cadena[i] <= '9') || cadena[i]=='.')) return false;
	}
	return true;
}

int CompararFecha(Fecha fecha1, Fecha fecha2)
{
	int val;

	if (fecha1.ano < fecha2.ano)
		val = -1;
	else if (fecha1.ano > fecha2.ano)
		val = 1;
	else {
		if (fecha1.mes < fecha2.mes)
			val = -1;
		else if (fecha1.mes > fecha2.mes)
			val = 1;
		else {
			if (fecha1.dia < fecha2.dia)
				val = -1;
			else if (fecha1.dia > fecha2.dia)
				val = 1;
			else
				val = 0;
		}
	}

	return val;
}

int CompararHora(Hora hora1, Hora hora2) {
	int val;

	if (hora1.hora < hora2.hora)
		val = -1;
	else if (hora1.hora > hora2.hora)
		val = 1;
	else {
		if (hora1.minuto < hora2.minuto)
			val = -1;
		else if (hora1.minuto > hora2.minuto)
			val = 1;
		else {
			val = 0;
		}
	}

	return val;
}

LRESULT CALLBACK procRegCitaFechaCita(HWND hWnd, UINT mensaje, WPARAM wParam, LPARAM lParam) {
	switch (mensaje)
	{
	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_TAB:
			SetFocus(GetDlgItem(GetDlgItem(hwndWndCitaNueva, GRP_DATOSCITA), DT_HORACITA));
			break;
		default:
			break;
		}
		break;
	default:
		return CallWindowProc(wndprocRegCitaFechaCita, hWnd, mensaje, wParam, lParam);
	}
}

LRESULT CALLBACK procRegCitaHoraCita(HWND hWnd, UINT mensaje, WPARAM wParam, LPARAM lParam) {
	switch (mensaje)
	{
	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_TAB:
			SetFocus(GetDlgItem(GetDlgItem(hwndWndCitaNueva, GRP_DATOSCITA), TXT_COSTOCITA));
			break;
		default:
			break;
		}
		break;
	default:
		return CallWindowProc(wndprocRegCitaHoraCita, hWnd, mensaje, wParam, lParam);
	}
}

LRESULT CALLBACK procRegCitaCostoCita(HWND hWnd, UINT mensaje, WPARAM wParam, LPARAM lParam) {
	switch (mensaje)
	{
	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_TAB:
			SetFocus(GetDlgItem(GetDlgItem(hwndWndCitaNueva, GRP_DATOSCITA), TXT_MOTIVOCONSULTA));
			break;
		default:
			break;
		}
		break;
	default:
		return CallWindowProc(wndprocRegCitaCostoCita, hWnd, mensaje, wParam, lParam);
	}
}

LRESULT CALLBACK procRegCitaMotivoCita(HWND hWnd, UINT mensaje, WPARAM wParam, LPARAM lParam) {
	switch (mensaje)
	{
	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_TAB:
			SetFocus(GetDlgItem(GetDlgItem(hwndWndCitaNueva, GRP_DATOSCLIENTE), TXT_NOMBRECLIENTE));
			break;
		default:
			break;
		}
		break;
	default:
		return CallWindowProc(wndprocRegCitaMotivoCita, hWnd, mensaje, wParam, lParam);
	}
}

LRESULT CALLBACK procRegCitaNombreCliente(HWND hWnd, UINT mensaje, WPARAM wParam, LPARAM lParam) {
	switch (mensaje)
	{
	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_TAB:
			SetFocus(GetDlgItem(GetDlgItem(hwndWndCitaNueva, GRP_DATOSCLIENTE), TXT_APELLIDOCLIENTE));
			break;
		default:
			break;
		}
		break;
	default:
		return CallWindowProc(wndprocRegCitaNombreCliente, hWnd, mensaje, wParam, lParam);
	}
}

LRESULT CALLBACK procRegCitaApellidoCliente(HWND hWnd, UINT mensaje, WPARAM wParam, LPARAM lParam) {
	switch (mensaje)
	{
	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_TAB:
			SetFocus(GetDlgItem(GetDlgItem(hwndWndCitaNueva, GRP_DATOSCLIENTE), TXT_TELEFONOCLIENTE));
			break;
		default:
			break;
		}
		break;
	default:
		return CallWindowProc(wndprocRegCitaApellidoCliente, hWnd, mensaje, wParam, lParam);
	}
}

LRESULT CALLBACK procRegCitaTelefonoCliente(HWND hWnd, UINT mensaje, WPARAM wParam, LPARAM lParam) {
	switch (mensaje)
	{
	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_TAB:
			SetFocus(GetDlgItem(GetDlgItem(hwndWndCitaNueva, GRP_DATOSMASCOTA), TXT_NOMBREMASCOTA));
			break;
		default:
			break;
		}
		break;
	default:
		return CallWindowProc(wndprocRegCitaTelefonoCliente, hWnd, mensaje, wParam, lParam);
	}
}

LRESULT CALLBACK procRegCitaNombreMascota(HWND hWnd, UINT mensaje, WPARAM wParam, LPARAM lParam) {
	switch (mensaje)
	{
	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_TAB:
			SetFocus(GetDlgItem(GetDlgItem(hwndWndCitaNueva, GRP_DATOSMASCOTA), CB_ESPECIEMASCOTA));
			break;
		default:
			break;
		}
		break;
	default:
		return CallWindowProc(wndprocRegCitaNombreMascota, hWnd, mensaje, wParam, lParam);
	}
}

void InsertarAgenda(byte accion, byte pagina, string filtro, HWND hWnd) {
	if (filtro != "") ShowWindow(GetDlgItem(hWnd, LBL_PAGINA_AGENDA), SW_HIDE);
	string strPag = "Página: ";
	strPag.append(to_string(pagina));
	SetWindowText(GetDlgItem(hWnd, LBL_PAGINA_AGENDA), strPag.c_str());

	if (origen == NULL) {
		CreateWindowEx(0, "STATIC", "Id",
			WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 20, 70, 50, 20, hWnd, (HMENU)LBL_AGENDA_ID, hInst, NULL);
		CreateWindowEx(0, "STATIC", "Fecha",
			WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 70, 70, 100, 20, hWnd, (HMENU)LBL_AGENDA_FECHA, hInst, NULL);
		CreateWindowEx(0, "STATIC", "Hora",
			WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 170, 70, 80, 20, hWnd, (HMENU)LBL_AGENDA_HORA, hInst, NULL);
		CreateWindowEx(0, "STATIC", "Nombre del cliente",
			WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 250, 70, 300, 20, hWnd, (HMENU)LBL_AGENDA_NOMBRECLIENTE, hInst, NULL);
		CreateWindowEx(0, "STATIC", "Acciones",
			WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 550, 70, 90, 20, hWnd, (HMENU)LBL_AGENDA_ACCIONES, hInst, NULL);

		HWND hwndTxtIdCita = CreateWindowEx(0, "STATIC", "La agenda se encuentra vacía",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | SS_CENTER,
			20, 90, 620, 20, hWnd, NULL, hInst, NULL);

		return;
	}

	//Limpiamos el GRID antes de empezar a insertar los datos
	if (accion == AGREGAR) {
		DestroyWindow(GetDlgItem(hWnd, LBL_AGENDA_ID));
		DestroyWindow(GetDlgItem(hWnd, LBL_AGENDA_FECHA));
		DestroyWindow(GetDlgItem(hWnd, LBL_AGENDA_HORA));
		DestroyWindow(GetDlgItem(hWnd, LBL_AGENDA_NOMBRECLIENTE));
		DestroyWindow(GetDlgItem(hWnd, LBL_AGENDA_ACCIONES));
		for (int i = 0; i < (citas - 1)*4; i++) {
			DestroyWindow(GetDlgItem(hWnd, LBL_AGENDAGRID));
		}
		for (int i = 0; i < citas - 1; i++) {
			DestroyWindow(GetDlgItem(hWnd, BTN_VERINFOCITA));
		}
		for (int i = 0; i < citas - 1; i++) {
			DestroyWindow(GetDlgItem(hWnd, BTN_EDITARCITA));
		}
		for (int i = 0; i < citas - 1; i++) {
			DestroyWindow(GetDlgItem(hWnd, BTN_CANCELARCITA));
		}
	}
	else if (accion == ELIMINAR) {
		DestroyWindow(GetDlgItem(hWnd, LBL_AGENDA_ID));
		DestroyWindow(GetDlgItem(hWnd, LBL_AGENDA_FECHA));
		DestroyWindow(GetDlgItem(hWnd, LBL_AGENDA_HORA));
		DestroyWindow(GetDlgItem(hWnd, LBL_AGENDA_NOMBRECLIENTE));
		DestroyWindow(GetDlgItem(hWnd, LBL_AGENDA_ACCIONES));

		for (int i = 0; i < 40; i++) {
			DestroyWindow(GetDlgItem(hWnd, LBL_AGENDAGRID));
			DestroyWindow(GetDlgItem(hWnd, BTN_VERINFOCITA));
			DestroyWindow(GetDlgItem(hWnd, BTN_EDITARCITA));
			DestroyWindow(GetDlgItem(hWnd, BTN_CANCELARCITA));
		}
		/*for (int i = 0; i < (citas + 1) * 4; i++) {
			DestroyWindow(GetDlgItem(hWnd, LBL_AGENDAGRID));
		}
		for (int i = 0; i < citas + 1; i++) {
			DestroyWindow(GetDlgItem(hWnd, BTN_VERINFOCITA));
		}
		for (int i = 0; i < citas + 1; i++) {
			DestroyWindow(GetDlgItem(hWnd, BTN_EDITARCITA));
		}
		for (int i = 0; i < citas + 1; i++) {
			DestroyWindow(GetDlgItem(hWnd, BTN_CANCELARCITA));
		}*/
	}
	else if (accion == EDITAR) {
		DestroyWindow(GetDlgItem(hWnd, LBL_AGENDA_ID));
		DestroyWindow(GetDlgItem(hWnd, LBL_AGENDA_FECHA));
		DestroyWindow(GetDlgItem(hWnd, LBL_AGENDA_HORA));
		DestroyWindow(GetDlgItem(hWnd, LBL_AGENDA_NOMBRECLIENTE));
		DestroyWindow(GetDlgItem(hWnd, LBL_AGENDA_ACCIONES));
		for (int i = 0; i < citas * 4; i++) {
			DestroyWindow(GetDlgItem(hWnd, LBL_AGENDAGRID));
		}
		for (int i = 0; i < citas; i++) {
			DestroyWindow(GetDlgItem(hWnd, BTN_VERINFOCITA));
		}
		for (int i = 0; i < citas; i++) {
			DestroyWindow(GetDlgItem(hWnd, BTN_EDITARCITA));
		}
		for (int i = 0; i < citas; i++) {
			DestroyWindow(GetDlgItem(hWnd, BTN_CANCELARCITA));
		}
	}

	//Insertamos los encabezados del GRID
	CreateWindowEx(0, "STATIC", "Id",
		WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 20, 70, 50, 20, hWnd, (HMENU)LBL_AGENDA_ID, hInst, NULL);
	CreateWindowEx(0, "STATIC", "Fecha",
		WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 70, 70, 100, 20, hWnd, (HMENU)LBL_AGENDA_FECHA, hInst, NULL);
	CreateWindowEx(0, "STATIC", "Hora",
		WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 170, 70, 80, 20, hWnd, (HMENU)LBL_AGENDA_HORA, hInst, NULL);
	CreateWindowEx(0, "STATIC", "Nombre del cliente",
		WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 250, 70, 300, 20, hWnd, (HMENU)LBL_AGENDA_NOMBRECLIENTE, hInst, NULL);
	CreateWindowEx(0, "STATIC", "Acciones",
		WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 550, 70, 90, 20, hWnd, (HMENU)LBL_AGENDA_ACCIONES, hInst, NULL);

	aux = origen;
	for (int i = 0; i < (pagina - 1)*REGISTROSPORPAGINA;i++) {
		aux = aux->sig;
	}
	
	for (int i = 0; i < REGISTROSPORPAGINA; i++) {
		if (i > REGISTROSPORPAGINA)break;
		if (aux == NULL) break;

		if (filtro != "") {
			if (aux->datosCita.fechaCita.fechaCompleta != filtro) {
				i--;
				aux = aux->sig;
				continue;
			}
		}

		HWND hwndTxtIdCita = CreateWindowEx(0, "STATIC", "",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | SS_CENTER,
			20, 90 + (i * 20), 50, 20, hWnd, (HMENU)LBL_AGENDAGRID, hInst, NULL);
		SetWindowText(hwndTxtIdCita, to_string(aux->datosCita.NumCita).c_str());

		HWND hwndTxtFecha = CreateWindowEx(0, "STATIC", "",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER|SS_CENTER,
			70, 90 +(i*20), 100, 20, hWnd, (HMENU)LBL_AGENDAGRID, hInst, NULL);
		SetWindowText(hwndTxtFecha, aux->datosCita.fechaCita.fechaCompleta.c_str());

		HWND hwndTxtHora = CreateWindowEx(0, "STATIC", "",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER|SS_CENTER,
			170, 90 + (i * 20), 80, 20, hWnd, (HMENU)LBL_AGENDAGRID, hInst, NULL);
		SetWindowText(hwndTxtHora, aux->datosCita.horaCita.horaCompleta.c_str());

		HWND hwndTxtNombreCliente = CreateWindowEx(0, "STATIC", "",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
			250, 90 + (i * 20), 300, 20, hWnd, (HMENU)LBL_AGENDAGRID, hInst, NULL);
		SetWindowText(hwndTxtNombreCliente, aux->datosCliente.nombreCompleto().c_str());

		HWND hwndBtnVerCita = CreateWindowEx(0,"BUTTON","",
			WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_BITMAP| BS_FLAT,
			550, 90 +(i*20),30,20, hWnd,(HMENU)BTN_VERINFOCITA,hInst,NULL);
		HBITMAP hbmBtnVer = (HBITMAP)LoadImage(NULL, "page.bmp", IMAGE_BITMAP, 12, 12, LR_LOADFROMFILE);
		SendMessage(hwndBtnVerCita, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbmBtnVer);

		HWND hwndBtnEditarCita = CreateWindowEx(0, "BUTTON", "",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_BITMAP| BS_FLAT,
			580, 90 + (i * 20), 30, 20, hWnd, (HMENU)BTN_EDITARCITA, hInst, NULL);
		HBITMAP hbmBtnEdit = (HBITMAP)LoadImage(NULL, "edit.bmp", IMAGE_BITMAP, 12, 12, LR_LOADFROMFILE);
		SendMessage(hwndBtnEditarCita, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbmBtnEdit);

		HWND hwndBtnEliminarCita = CreateWindowEx(0, "BUTTON", "",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_BITMAP| BS_FLAT,
			610, 90 + (i * 20), 30, 20, hWnd, (HMENU)BTN_CANCELARCITA, hInst, NULL);
		HBITMAP hbmBtnEliminar = (HBITMAP)LoadImage(NULL, "cancel.bmp", IMAGE_BITMAP, 12, 12, LR_LOADFROMFILE);
		SendMessage(hwndBtnEliminarCita, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbmBtnEliminar);
		
		aux = aux->sig;
	}

	////Creamos los botones de paginación
	//bool tieneDecimales=false;
	//int nNumPags;

	////Limpiamos los botones de paginación
	//if (accion==SIMPLE) {
	//	tieneDecimales = false;
	//	bool pasoPunto = false;
	//	float numPags = ((float)citas) / REGISTROSPORPAGINA;
	//	nNumPags = 0;
	//	string strNumPags = to_string(numPags);
	//	for (int i = 0; i < strNumPags.size(); i++) {
	//		if (pasoPunto == true && strNumPags[i] != '0') { tieneDecimales = true; break; };
	//		if (strNumPags[i] == '.') pasoPunto = true;
	//	}
	//	if (tieneDecimales == true) {
	//		nNumPags = numPags + 1;
	//	}
	//}else if (accion == AGREGAR) {
	//	tieneDecimales = false;
	//	bool pasoPunto = false;
	//	double numPags = ((double)citas-1) / REGISTROSPORPAGINA;
	//	nNumPags = 0;
	//	string strNumPags = to_string(numPags);
	//	for (int i = 0; i < strNumPags.size(); i++) {
	//		if (pasoPunto == true && strNumPags[i] != '0') { tieneDecimales = true; break; };
	//		if (strNumPags[i] == '.') pasoPunto = true;
	//	}
	//	if (tieneDecimales == true) {
	//		nNumPags = numPags + 1;
	//	}

	//	for (int i = 0; i < nNumPags; i++) {
	//		DestroyWindow(GetDlgItem(hWnd, BTN_PAGINACION));
	//	}
	//}
	//else if (accion == ELIMINAR) {
	//	tieneDecimales = false;
	//	bool pasoPunto = false;
	//	double numPags = ((double)citas+1) / REGISTROSPORPAGINA;
	//	nNumPags = 0;
	//	string strNumPags = to_string(numPags);
	//	for (int i = 0; i < strNumPags.size(); i++) {
	//		if (pasoPunto == true && strNumPags[i] != '0') { tieneDecimales = true; break; };
	//		if (strNumPags[i] == '.') pasoPunto = true;
	//	}
	//	if (tieneDecimales == true) {
	//		nNumPags = numPags + 1;
	//	}

	//	for (int i = 0; i < nNumPags; i++) {
	//		DestroyWindow(GetDlgItem(hWnd, BTN_PAGINACION));
	//	}
	//}
	//else if (accion == EDITAR) {
	//	tieneDecimales = false;
	//	bool pasoPunto = false;
	//	double numPags = ((double)citas) / REGISTROSPORPAGINA;
	//	nNumPags = 0;
	//	string strNumPags = to_string(numPags);
	//	for (int i = 0; i < strNumPags.size(); i++) {
	//		if (pasoPunto == true && strNumPags[i] != '0') { tieneDecimales = true; break; };
	//		if (strNumPags[i] == '.') pasoPunto = true;
	//	}
	//	if (tieneDecimales == true) {
	//		nNumPags = numPags + 1;
	//	}

	//	for (int i = 0; i < nNumPags;i++) {
	//		DestroyWindow(GetDlgItem(hWnd, BTN_PAGINACION));
	//	}
	//}

	//if (filtro != "") return;

	//tieneDecimales = false;
	//bool pasoPunto = false;
	//double numPags = ((double)citas) / REGISTROSPORPAGINA;
	//nNumPags = 0;
	//string strNumPags = to_string(numPags);
	//for (int i = 0; i < strNumPags.size(); i++) {
	//	if (pasoPunto == true && strNumPags[i] != '0') { tieneDecimales = true; break; };
	//	if (strNumPags[i] == '.') pasoPunto = true;
	//}
	//if (tieneDecimales == true) {
	//	nNumPags = numPags + 1;
	//}
	//else {
	//	nNumPags = numPags;
	//}

	//for (int i = 0; i < nNumPags; i++) {
	//	string pag = to_string(i+1);
	//	if (i + 1 == pagina) {
	//		HWND hwndBtnVerCita = CreateWindowEx(0, "BUTTON", pag.c_str(),
	//			WS_CHILD | WS_VISIBLE | BS_FLAT | BS_CENTER | WS_DISABLED,
	//			20 + (i * 30), (REGISTROSPORPAGINA * 20) + 95, 25, 25, hWnd, (HMENU)BTN_PAGINACION, hInst, NULL);
	//	}
	//	else {
	//		HWND hwndBtnVerCita = CreateWindowEx(0, "BUTTON", pag.c_str(),
	//			WS_CHILD | WS_VISIBLE | BS_FLAT | BS_CENTER,
	//			20 + (i * 30), (REGISTROSPORPAGINA * 20) + 95, 25, 25, hWnd, (HMENU)BTN_PAGINACION, hInst, NULL);
	//	}
	//}
	
}

void CargarDatosDoctor() {
	fstream archivo;
	archivo.open("DatosDoctor.txt");
	if (archivo.is_open()) {
		string linea;

		archivo.eof();
		getline(archivo, linea);
		doctor.cedula = linea;

		archivo.eof();
		getline(archivo, linea);
		doctor.nombre = linea;

		archivo.eof();
		getline(archivo, linea);
		doctor.apellido = linea;

		archivo.eof();
		getline(archivo, linea);
		doctor.telefono = linea;

		archivo.eof();
		getline(archivo, linea);
		doctor.correo = linea;

		archivo.eof();
		getline(archivo, linea);
		doctor.rutaFoto = linea;

		archivo.close();
	}
}

Cita *BuscarCitaPorId(int id) {
	Cita *cita=NULL;
	aux = origen;
	while (aux != NULL) {
		if (aux->datosCita.NumCita == id) {
			cita = aux;
			break;
		}
		aux = aux->sig;
	}
	return cita;
}

LRESULT CALLBACK procResumenCita(HWND hWnd, UINT mensaje, WPARAM wParam, LPARAM lParam) {
	switch (mensaje)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case BTN_SALIRRESUMEN:
			DestroyWindow(hWnd);
			break;
		}
		break;
		//Este case permite que se pueda arrastrar la ventana desde cualquier punto
	case WM_NCHITTEST:
	{
		//Si tomamos el mensaje que se envia al interactuar con el cliente y lo enviamos como si fuera el mensaje
		//de interactuar con el encabezado de la ventana
		const LRESULT result = ::DefWindowProc(hWnd, mensaje, wParam, lParam);
		return (result == HTCLIENT) ? HTCAPTION : result;
	}
	case WM_PAINT:
	{
		HDC dcMem=NULL, dcCliente=NULL;
		PAINTSTRUCT ps;
		HBITMAP hbmFondo = (HBITMAP)LoadImage(NULL, "libreta.bmp", IMAGE_BITMAP, 348, 402, LR_LOADFROMFILE);
		dcCliente = BeginPaint(hWnd, &ps);
		dcMem = CreateCompatibleDC(dcCliente);
		BITMAP bmFondo;
		SelectObject(dcMem, hbmFondo);
		GetObject(hbmFondo, sizeof(BITMAP), &bmFondo);
		BitBlt(dcCliente, 0, 0, 348, 402, dcMem, 0, 0, SRCCOPY);
		DeleteDC(dcMem);
		EndPaint(hWnd, &ps);
		DeleteObject(hbmFondo);

	}
		break;
	case WM_CTLCOLORSTATIC:
	{
		HBRUSH hbrStatic = CreateSolidBrush(RGB(255, 255, 255));
		HDC hdcStatic = (HDC)wParam;
		SetBkColor(hdcStatic, RGB(255, 255, 255));
		return (LRESULT)hbrStatic;
	}
	case WM_CREATE:
	{
		InsertarControlesResumenCita(hWnd);
	}
	break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, mensaje, wParam, lParam);
	}
}

void InsertarControlesResumenCita(HWND hWnd) {
	HWND hwndBtnCerrar = CreateWindowEx(0, "BUTTON", "",
		WS_VISIBLE | WS_CHILD | BS_BITMAP | BS_FLAT,
		321, 5, 20, 20, hWnd, (HMENU)BTN_SALIRRESUMEN, hInst, NULL);
	HBITMAP hbmImgCerrar = (HBITMAP)LoadImage(NULL, "cancel.bmp", IMAGE_BITMAP, 15, 15, LR_LOADFROMFILE);
	SendMessage(hwndBtnCerrar, BM_SETIMAGE, 0, (LPARAM)hbmImgCerrar);

	CreateWindowEx(0, "STATIC", "No. cita: ",
		WS_CHILD | WS_VISIBLE,
		10, 7, 60, 16, hWnd, NULL, hInst, NULL);
	CreateWindowEx(0, "STATIC", "",
		WS_CHILD | WS_VISIBLE,
		70, 7, 100, 16, hWnd, (HMENU)LBL_RESUMEN_NUMCITA, hInst, NULL);

	CreateWindowEx(0, "STATIC", "Fecha: ",
		WS_CHILD | WS_VISIBLE,
		180, 7, 50, 16, hWnd, NULL, hInst, NULL);
	CreateWindowEx(0, "STATIC", "",
		WS_CHILD | WS_VISIBLE,
		230, 7, 80, 16, hWnd, (HMENU)LBL_RESUMEN_FECHA, hInst, NULL);

	CreateWindowEx(0, "STATIC", "Hora: ",
		WS_CHILD | WS_VISIBLE,
		180, 25, 50, 16, hWnd, NULL, hInst, NULL);
	CreateWindowEx(0, "STATIC", "",
		WS_CHILD | WS_VISIBLE,
		230, 25, 50, 16, hWnd, (HMENU)LBL_RESUMEN_HORA, hInst, NULL);

	CreateWindowEx(0, "STATIC", "",
		WS_CHILD | WS_VISIBLE,
		25, 56, 300, 16, hWnd, (HMENU)LBL_RESUMEN_NOMBRECLIENTE, hInst, NULL);

	CreateWindowEx(0, "STATIC", "",
		WS_CHILD | WS_VISIBLE,
		25, 74, 200, 16, hWnd, (HMENU)LBL_RESUMEN_TELEFONO, hInst, NULL);

	CreateWindowEx(0, "STATIC", "Mascota: ",
		WS_CHILD | WS_VISIBLE,
		25, 110, 65, 16, hWnd, NULL, hInst, NULL);
	CreateWindowEx(0, "STATIC", "",
		WS_CHILD | WS_VISIBLE,
		90, 110, 100, 16, hWnd, (HMENU)LBL_RESUMEN_NOMBREMASCOTA, hInst, NULL);
	CreateWindowEx(0, "STATIC", "",
		WS_CHILD | WS_VISIBLE,
		25, 128, 100, 16, hWnd, (HMENU)LBL_RESUMEN_ESPECIE, hInst, NULL);

	CreateWindowEx(0, "STATIC", "Notas: ",
		WS_CHILD | WS_VISIBLE,
		25, 164, 80, 16, hWnd, NULL, hInst, NULL);
	CreateWindowEx(0, "STATIC", "",
		WS_CHILD | WS_VISIBLE,
		25, 182, 313, 32, hWnd, (HMENU)LBL_RESUMEN_MOTIVO, hInst, NULL);

	CreateWindowEx(0, "STATIC", "Costo: ",
		WS_CHILD | WS_VISIBLE,
		25, 270, 50, 16, hWnd, NULL, hInst, NULL);
	CreateWindowEx(0, "STATIC", "",
		WS_CHILD | WS_VISIBLE,
		75, 270, 100, 16, hWnd, (HMENU)LBL_RESUMEN_COSTO, hInst, NULL);
}

LRESULT CALLBACK procGrpCita(HWND hWnd, UINT mensaje, WPARAM wParam, LPARAM lParam) {
	switch (mensaje)
	{
	case WM_CTLCOLORSTATIC:
	{
		HBRUSH hbrStatics = CreateSolidBrush(RGB(180, 180, 180));
		SetBkColor((HDC)wParam, RGB(180, 180, 180));
		return (LRESULT)hbrStatics;
	}
	default:
		return CallWindowProc(wndprocGrpCita, hWnd, mensaje, wParam, lParam);
	}
}

LRESULT CALLBACK procGrpCliente(HWND hWnd, UINT mensaje, WPARAM wParam, LPARAM lParam) {
	switch (mensaje)
	{
	case WM_CTLCOLORSTATIC:
	{
		HBRUSH hbrStatics = CreateSolidBrush(RGB(180, 180, 180));
		SetBkColor((HDC)wParam, RGB(180, 180, 180));
		return (LRESULT)hbrStatics;
	}
	default:
		return CallWindowProc(wndprocGrpCliente, hWnd, mensaje, wParam, lParam);
	}
}

LRESULT CALLBACK procGrpMascota(HWND hWnd, UINT mensaje, WPARAM wParam, LPARAM lParam) {
	switch (mensaje)
	{
	case WM_CTLCOLORSTATIC:
	{
		HBRUSH hbrStatics = CreateSolidBrush(RGB(180, 180, 180));
		SetBkColor((HDC)wParam, RGB(180, 180, 180));
		return (LRESULT)hbrStatics;
	}
	default:
		return CallWindowProc(wndprocGrpMascota, hWnd, mensaje, wParam, lParam);
	}
}

void ActualizarCita() {
	if (!ValidarCita()) return;
	//Variables
	string sTexto;
	char textoCorto[255], textoLargo[1024];

	//Variables de datos de la cita
	unsigned int numCita;
	byte dia, mes, hora, minuto;
	unsigned int ano;
	float costo;
	string strFecha, strHora, motivo;

	//Variables de datos del cliente
	string nombreCliente, apellidoCliente, telefonoCliente;
	//Variables de datos de la mascota
	string nombreMascota, especieMascota;
	bool esMacho;

	HWND hwndGrpDatosCita = GetDlgItem(hwndWndCitaNueva, GRP_DATOSCITA);
	HWND hwndGrpDatosCliente = GetDlgItem(hwndWndCitaNueva, GRP_DATOSCLIENTE);
	HWND hwndGrpDatosMascota = GetDlgItem(hwndWndCitaNueva, GRP_DATOSMASCOTA);

	GetWindowText(GetDlgItem(hwndGrpDatosCita, TXT_NUMEROCITA), textoCorto, 255);
	numCita = atoi(textoCorto);
	GetWindowText(GetDlgItem(hwndGrpDatosCita, DT_FECHACITA), textoCorto, 255);
	strFecha = textoCorto;
	sTexto = textoCorto;
	DesglozarFecha(sTexto, &dia, &mes, &ano);
	GetWindowText(GetDlgItem(hwndGrpDatosCita, DT_HORACITA), textoCorto, 255);
	strHora = textoCorto;
	sTexto = textoCorto;
	DesglozarTiempo(sTexto, &hora, &minuto);
	GetWindowText(GetDlgItem(hwndGrpDatosCita, TXT_COSTOCITA), textoCorto, 255);
	costo = stof(textoCorto);
	GetWindowText(GetDlgItem(hwndGrpDatosCita, TXT_MOTIVOCONSULTA), textoCorto, 255);
	motivo = textoCorto;

	GetWindowText(GetDlgItem(hwndGrpDatosCliente, TXT_NOMBRECLIENTE), textoCorto, 255);
	nombreCliente = textoCorto;
	GetWindowText(GetDlgItem(hwndGrpDatosCliente, TXT_APELLIDOCLIENTE), textoCorto, 255);
	apellidoCliente = textoCorto;
	GetWindowText(GetDlgItem(hwndGrpDatosCliente, TXT_TELEFONOCLIENTE), textoCorto, 255);
	telefonoCliente = textoCorto;

	GetWindowText(GetDlgItem(hwndGrpDatosMascota, TXT_NOMBREMASCOTA), textoCorto, 255);
	nombreMascota = textoCorto;
	GetWindowText(GetDlgItem(hwndGrpDatosMascota, CB_ESPECIEMASCOTA), textoCorto, 255);
	especieMascota = textoCorto;
	if (IsDlgButtonChecked(hwndGrpDatosMascota, RB_MACHOMASCOTA) == BST_CHECKED)
		esMacho = true;
	else
		esMacho = false;

	aux = origen;
	while (aux->sig != NULL) {
		if (aux->datosCita.NumCita == numCita) break;
		aux = aux->sig;
	}

	aux->datosCita.costo = costo;
	aux->datosCita.motivoConsulta = motivo;
	aux->datosCliente.nombre = nombreCliente;
	aux->datosCliente.apellido = apellidoCliente;
	aux->datosCliente.telefono = telefonoCliente;
	aux->datosMascota.nombre = nombreMascota;
	aux->datosMascota.especie = especieMascota;
	if (esMacho == true)
		aux->datosMascota.esMacho = true;
	else
		aux->datosMascota.esMacho = false;

	InsertarAgenda(EDITAR, 1, "", hwndWndAgenda);
	DestroyWindow(hwndWndCitaNueva);
	Mensajes::OperacionExitosa("Se han actualizado los datos correctamente");
}