package fr.eurecom.senml.entity;

import fr.eurecom.senml.entity.Measure.ParamTypeValue;

public interface ISensorAdmin extends ISensor {
	
	public boolean linkZone(IZone zone);
	
	public boolean linkZone(String zoneKey);
	
	public void unlinkZone();

	public Measure addMeasure(String measureName, 
			Units measureUnit, 
			ParamTypeValue measureTypeValue, 
			String measureValue, long timeMeasure, String type);
	
	public void deleteMeasure(String measureKey);
	
	public void deleteMeasure(Measure measure);

	public void setTitle(String title);

//	void tag(Tag tag);

//	void untag(Tag tag);

	public void destroy();

	public Measure addMeasure(String measureName, Units measureUnit,
			ParamTypeValue measureTypeValue, String measureValue,
			long timeMeasure, String type, String sum);

}
