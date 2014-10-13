package fr.eurecom.senml.entity;


public interface IZoneAdmin extends IZone {
	
	public boolean linkParentZone(String parentZoneKey);
	
	public boolean linkParentZone(IZone parentZone);
	
	public void unlinkParentZone();

//	public boolean addSensor(String sensorKey);

//	public boolean removeSensor(String sensorKey);

	public void destroy();
}
