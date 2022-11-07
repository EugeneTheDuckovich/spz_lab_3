#include "driver.h"

UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\lab3_brigade11_driver");
UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(L"\\??\\lab3_brigade11_driver_link");

PDEVICE_OBJECT DeviceObject = NULL;

NTSTATUS DispatchPassThru(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status = STATUS_SUCCESS;

	switch (irpsp->MajorFunction) {
	case IRP_MJ_CREATE:
		DbgPrintEx(0, 0, ("opening request was created  \r\n"));
		break;
	case IRP_MJ_CLOSE:
		DbgPrintEx(0, 0, ("closing request was created  \r\n"));
		break;
	case IRP_MJ_READ:
		DbgPrintEx(0, 0, ("reading request was created  \r\n"));
		break;
	default:
		break;
	}

	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status;
}

NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT driverObject,
	_In_ PUNICODE_STRING registryPath
)
{
	DbgPrintEx(0, 0, ("[lab3]"__FUNCTION__ ));
	UNREFERENCED_PARAMETER(registryPath);

	NTSTATUS status;

	driverObject->DriverUnload = DriverUnload;

	status = IoCreateDevice(driverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);

	if (!NT_SUCCESS(status)) {
		DbgPrintEx(0, 0, ("unable to create a device  \r\n"));
		return status;
	}

	status = IoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);

	if (!NT_SUCCESS(status)) {
		DbgPrintEx(0, 0, ("unable to create a symbolic link  \r\n"));
		IoDeleteDevice(DeviceObject);

		return status;
	}

	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i) {
		driverObject->MajorFunction[i] = DispatchPassThru;
	}

	// driverObject->MajorFunction[IRP_MJ_READ] = DispatchCustom;
	// driverObject->MajorFunction[IRP_MJ_WRITE] = DispatchCustom;


	DbgPrintEx(0, 0, ("lab3 loading has been commited  \r\n"));

	RTL_OSVERSIONINFOW operationSystemData;
	RtlGetVersion(&operationSystemData);
	// Print data about os
	DbgPrintEx(0, 0, "System version: windows %u %u.%u  \r\n",
		(unsigned int)operationSystemData.dwBuildNumber,
		(unsigned int)operationSystemData.dwMajorVersion,
		(unsigned int)operationSystemData.dwMinorVersion);

	return status;
}

VOID DriverUnload(
	_In_ PDRIVER_OBJECT driverObject)
{
	IoDeleteSymbolicLink(&SymbolicLinkName);
	IoDeleteDevice(DeviceObject);

	DbgPrintEx(0, 0, ("lab3 unloading has been commited  \r\n"));
}