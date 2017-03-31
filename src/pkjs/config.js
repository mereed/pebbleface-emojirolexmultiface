module.exports = [
  {
    "type": "heading",
    "defaultValue": "Configuration"
  },
     {
        "type": "text",
        "defaultValue": "<h6>A white button = OFF, an orange button = ON</h6>",
      },
	{
    "type": "section",
    "items": [
 	{
        "type": "heading",
        "defaultValue": "<h5>Display</h5>"
	},
     {
        "type": "color",
        "messageKey": "hrcol",
        "defaultValue": "0xFFFFFF",
        "label": "Hour Hand Color"
      },
      {
        "type": "color",
        "messageKey": "mincol",
        "defaultValue": "0xFFFFFF",
        "label": "Minute Hand Colour"
      },
      {
        "type": "color",
        "messageKey": "seccol",
        "defaultValue": "0xFF0000",
        "label": "Second Hand Colour"
	  },
	  {
        "type": "toggle",
        "messageKey": "date",
        "label": "Show date",
        "defaultValue": true
      },
	  {
        "type": "toggle",
        "messageKey": "secs",
        "label": "Show second hand",
        "defaultValue": true
      },
	  {
        "type": "toggle",
        "messageKey": "batt",
        "label": "Show battery number",
        "defaultValue": true
      },	
	  {
  "type": "select",
  "messageKey": "background",
  "defaultValue": 0,
  "label": "Rolex style",
  "options": [
    { 
		"label": "Explorer (metallic) (date)",
      "value": 0
    },
    { 
		"label": "Explorer (black) (date)",
      "value": 1
    },
	  { 
		  "label": "Explorer II (no date)",
      "value": 2
    },
	{ 
		"label": "Submaniner (no date)",
      "value": 3
    },
	{ 
		"label": "Commando (no date)",
      "value": 4
    },
    { 
      "label": "Date",
      "value": 5
    },
    { 
		"label": "Sea dweller (date)",
      "value": 6
    }
  ]
},
]		
},
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  },
  {
        "type": "text",
        "defaultValue": "<h6>If you find this watchface useful, please consider making a <a href='https://www.paypal.me/markchopsreed'>small donation</a>. Thankyou.</h6>",
  }
];