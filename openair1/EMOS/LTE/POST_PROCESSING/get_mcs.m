function mcs = get_mcs(dci_pdu, format)
% mcs = get_mcs(dci_pdu,format)
%   dci_pdu is a vector of unit16 containing
%   format is one of the following strings
%         {format0,
%     	   format1,
%     	   format1A,
%     	   format1B,
%     	   format1C,
%     	   format1D,
%     	   format2,
%     	   format2_2A_L10PRB,
%     	   format2_2A_M10PRB,
%     	   format2_4A_L10PRB,
%     	   format2_4A_M10PRB,
%     	   format2A_2A_L10PRB,
%     	   format2A_2A_M10PRB,
%     	   format2A_4A_L10PRB,
%     	   format2A_4A_M10PRB,
%     	   format3};

if nargin < 2
    format = 'format2A_2A_M10PRB';
end

switch format
    case 'format2A_2A_M10PRB'
        % this is for format type 2A, 5MHz, TDD, 2 antenna ports more than 10 PRB,
        % 42 bits
        % the mcs is in bits 24-28, so in bits 8-12 of the second word of the pdu
        mask = uint16(0);
        mask = bitset(mask,8);
        mask = bitset(mask,9);
        mask = bitset(mask,10);
        mask = bitset(mask,11);
        mask = bitset(mask,12);
        mcs = bitand(mask,dci_pdu(2));
        mcs = bitshift(mcs,-8);
    case 'format0'
        % this is for format 0, 5MHz, TDD, 27 bits
        % the mcs is in bits 11-15
        mask = uint16(0);
        mask = bitset(mask,11);
        mask = bitset(mask,12);
        mask = bitset(mask,13);
        mask = bitset(mask,14);
        mask = bitset(mask,15);
        mcs = bitand(mask,dci_pdu(1));
        mcs = bitshift(mcs,-11);
    otherwise
        error('format not yet implemented or wrong');
end
