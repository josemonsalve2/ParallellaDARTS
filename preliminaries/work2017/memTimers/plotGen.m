[~, idx,idxb]=unique(results(:,1:2),'rows','stable')
resultsArr = table2array(results);
val = accumarray(idxb,resultsArr(:,3),[],@std);
posit = resultsArr(1:16,1:2);
val_norm = val
val_norm_mat = vec2mat(val_norm, 4)
imagesc(val_norm_mat)
colorbar
textStrings = num2str(val_norm_mat(:),'%0.2f')
textStrings = strtrim(cellstr(textStrings))
[x,y] = meshgrid(1:4);
hStrings = text(x(:),y(:),textStrings(:),'HorizontalAlignment','center');
title(sprintf('1000 FMUL operations from core 0,0 Standard Deviation\nAll cores concurrently working\nEach block is a core'))
xticks([1 2 3 4])
yticks([1 2 3 4])
set(gca,'XAxisLocation','top','YAxisLocation','left','ydir','reverse');