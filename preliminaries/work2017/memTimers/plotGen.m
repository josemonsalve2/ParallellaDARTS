[~, idx,idxb]=unique(results(:,1:2),'rows','stable')
resultsArr = table2array(results);
val = accumarray(idxb,resultsArr(:,3),[],@mean);
posit = resultsArr(1:16,1:2);
val_norm = val./val(1)
val_norm_mat = vec2mat(val_norm, 4)
imagesc(val_norm_mat)
colorbar
textStrings = num2str(val_norm_mat(:),'%0.2fx')
textStrings = strtrim(cellstr(textStrings))
[x,y] = meshgrid(1:4);
hStrings = text(x(:),y(:),textStrings(:),'HorizontalAlignment','center');