pathname = '/homes/latif/devel/openair_lte/openair1/EMOS/LTE/POST_PROCESSING/Mode5';
d = dir([pathname '/*.mat']);
filenames = {d.name};
SNR_4_4_cat = [];
chcap_mode5_4_4_cat = [];
chcap_mode5_4_4_user1_cat = [];
chcap_mode5_4_4_user2_cat = [];

SNR_4_16_cat = [];
chcap_mode5_4_16_cat = [];
chcap_mode5_4_16_user1_cat = [];
chcap_mode5_4_16_user2_cat = [];

SNR_16_4_cat = [];
chcap_mode5_16_4_cat = [];
chcap_mode5_16_4_user1_cat = [];
chcap_mode5_16_4_user2_cat = [];

SNR_16_16_cat = [];
chcap_mode5_16_16_cat = [];
chcap_mode5_16_16_user1_cat = [];
chcap_mode5_16_16_user2_cat = [];

for i = 1:length(filenames)
    
    load(filenames{i})
    
    if (M1 == 4 && M2 == 4)
        
        SNR_4_4_cat =[SNR_4_4_cat SNR];
        chcap_mode5_4_4_cat = [chcap_mode5_4_4_cat chcap_lte_sch];
        chcap_mode5_4_4_user1_cat = [chcap_mode5_4_4_user1_cat chcap_lte_sch1];
        chcap_mode5_4_4_user2_cat = [chcap_mode5_4_4_user2_cat chcap_lte_sch2];
       
    
    else if (M1 == 4 && M2 == 16)
            SNR_4_16_cat =[SNR_4_16_cat SNR];
            chcap_mode5_4_16_cat = [chcap_mode5_4_16_cat chcap_lte_sch];
            chcap_mode5_4_16_user1_cat = [chcap_mode5_4_16_user1_cat chcap_lte_sch1];
            chcap_mode5_4_16_user2_cat = [chcap_mode5_4_16_user2_cat chcap_lte_sch2];
            
        else if (M1 == 16 && M2 == 4)
                SNR_16_4_cat =[SNR_16_4_cat SNR];
                chcap_mode5_16_4_cat = [chcap_mode5_16_4_cat chcap_lte_sch];
                chcap_mode5_16_4_user1_cat = [chcap_mode5_16_4_user1_cat chcap_lte_sch1];
                chcap_mode5_16_4_user2_cat = [chcap_mode5_16_4_user2_cat chcap_lte_sch2];
            else
                SNR_16_16_cat =[SNR_16_4_cat SNR];
                chcap_mode5_16_16_cat = [chcap_mode5_16_16_cat chcap_lte_sch];
                chcap_mode5_16_16_user1_cat = [chcap_mode5_16_16_user1_cat chcap_lte_sch1];
                chcap_mode5_16_16_user2_cat = [chcap_mode5_16_16_user2_cat chcap_lte_sch2];
            end
        end
    end
    
end

save('table_LTE_MODE_4_5.mat', 'SNR_4_4_cat', 'chcap_mode5_4_4_cat', 'chcap_mode5_4_4_user1_cat', 'chcap_mode5_4_4_user2_cat', ...
                               'SNR_4_16_cat', 'chcap_mode5_4_16_cat', 'chcap_mode5_4_16_user1_cat', 'chcap_mode5_4_16_user2_cat', ...
                               'SNR_16_4_cat', 'chcap_mode5_16_4_cat', 'chcap_mode5_16_4_user1_cat', 'chcap_mode5_16_4_user2_cat', ...
                               'SNR_16_16_cat', 'chcap_mode5_16_16_cat', 'chcap_mode5_16_16_user1_cat', 'chcap_mode5_16_16_user2_cat')
                           